#include <stdlib.h>
#include "osci.h"
#include "lcd.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "touch_key.h"

volatile bool adc_get_flag = false; 
uint32_t* adc_value;

arm_cfft_radix4_instance_f32 scfft;
uint32_t tim_prescaler = 21;  //168M / 21 = 8M
uint32_t tim_period = 10; //8M/10 = 800K

void Osci_Show(void)
{
    if(adc_get_flag == true) //ADC采样1024完毕
    {
        uint16_t i;
        float fft_input[1024*2]; //fft的输入数组（fft需要输入复数，所以输入数组是采样数据的2倍）
        float fft_output[1024]; //fft输出数组
        uint32_t fft_max_index;
        float fft_max_value;
        float freq;
        uint32_t max, max_index, min, min_index;
        int32_t mid;
        uint32_t diff_sum;
        static uint8_t num;
        float max_true, min_true;

        __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);
        HAL_TIM_Base_Stop(&htim8);
        HAL_ADC_Stop_DMA(&hadc1);  //采样完毕之后需要清除定时器中断标志，停止定时器，停止DMA传输

        if(num < 3)
        {
            Lcd_Clear(BLACK);  //前两次运行先清除全屏
            num++;
        }
        else
        {
            Lcd_Clear_Osci(BLACK);  //之后只需要清除波形显示部分的屏幕
        }

        arm_max_q31((q31_t *)adc_value, 320, (q31_t *)&max, &max_index);
        arm_min_q31((q31_t *)adc_value, 320, (q31_t *)&min, &min_index); //在adc采样的原数据的前320个数据中寻找最大值和最小值
        mid = min + (max - min) / 2; //求中间值

        diff_sum = 0;
        for(i=0; i< 320; i++)
        {
            diff_sum += abs((int32_t)(adc_value[i] - mid));
        }
        diff_sum = diff_sum / 320; //计算前320个数据相对中间值的平均偏差
        
        if(diff_sum <= 2) //如果这320个数据相对中间值的平均偏差小于2
        {
            freq = 0.0f; //则认为波形为直流信号，频率为0
        }
        else
        {
            for(i=0; i<1024; i++)
            {
                fft_input[i*2] = (adc_value[i] - ZERO_VALUE) / RESOLU;  //fft输入数组前1024个数放实部（将adc值转成真实电压值）
                fft_input[i*2+1] = 0;  //fft输入数组后1024个数放虚部（虚部全为0）
            }
            arm_cfft_radix4_f32(&scfft, fft_input);  //进行快速傅里叶变换

            arm_cmplx_mag_f32(fft_input, fft_output, 1024);  //输出傅里叶变换后的频谱

            fft_output[0] = 0.0f;  //清零频谱数组的第1个数（因为上面的程序已经判断了这个信号不是直流信号，所以可以直接去除直流分量）
            arm_max_f32(fft_output, 512, &fft_max_value, &fft_max_index);  //取出频谱数组中的最大值
            freq = 168000000.0f / tim_period / tim_prescaler / 1024.0f * fft_max_index / 1000.0f; //计算频率：采样频率/采样点数*最大值的下标 （计算的单位是HZ）
        }
        
        for(i=0; i<320; i++)  //波形描点
        {
            adc_value[i] = adc_value[i] > 220 ? 220 : adc_value[i];
            Lcd_Draw_Point(i, 240-adc_value[i], YELLOW);
            Lcd_Draw_Point(i, 17, GREEN);
            Lcd_Draw_Point(i, 167, WHITE);
        }
        max_index = max_index > 3 ? max_index : 3;
        min_index = min_index > 3 ? min_index : 3;

        Lcd_ShowChar(max_index-3, 240-(adc_value[max_index]+6), '*', SHOW_SIZE_16, RED);
        Lcd_ShowChar(min_index-3, 240-(adc_value[min_index]+6), '*', SHOW_SIZE_16, GBLUE);  //标记出最大值和最小值

        max_true = (max - ZERO_VALUE) / RESOLU;
        min_true = (min - ZERO_VALUE) / RESOLU;
        max_true = max_true * VOLTAGE_COEFF;
        min_true = min_true * VOLTAGE_COEFF;  //计算真实电压值

        Lcd_Osci_printf("Max:%1.1fV Min:%1.1fV Freq:%2.2fkHz   ", max_true, min_true, freq);  //打印波形参数数据

        HAL_Delay(1000);
        HAL_ADC_Start_DMA(&hadc1, adc_value, 1024);  //重新进行下一周期采样
        HAL_TIM_Base_Start(&htim8);
        adc_get_flag = false;
    } 

    if(Touch_Key_Value != TOUCH_KEY_NONE)
    {
        switch (Touch_Key_Value)
        {
            case TOUCH_KEY_UP:  //触摸按键“上”键增大采样周期，一次增大10%
                {
                    tim_prescaler += tim_prescaler / 10;
                    __HAL_TIM_SET_PRESCALER(&htim8, tim_prescaler);
                }
            break;
            case TOUCH_KEY_DOWN:  //触摸按键“下”键减小采样周期一次减小10%
                {
                    if(tim_prescaler > 10)
                    {
                        tim_prescaler -= tim_prescaler / 10;
                        __HAL_TIM_SET_PRESCALER(&htim8, tim_prescaler);
                    }
                }
            break;
            
            default:
                break;
        }
        Touch_Key_Value = TOUCH_KEY_NONE;
    }
}
