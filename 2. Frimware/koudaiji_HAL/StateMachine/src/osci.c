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
    if(adc_get_flag == true) //ADC����1024���
    {
        uint16_t i;
        float fft_input[1024*2]; //fft���������飨fft��Ҫ���븴�����������������ǲ������ݵ�2����
        float fft_output[1024]; //fft�������
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
        HAL_ADC_Stop_DMA(&hadc1);  //�������֮����Ҫ�����ʱ���жϱ�־��ֹͣ��ʱ����ֹͣDMA����

        if(num < 3)
        {
            Lcd_Clear(BLACK);  //ǰ�������������ȫ��
            num++;
        }
        else
        {
            Lcd_Clear_Osci(BLACK);  //֮��ֻ��Ҫ���������ʾ���ֵ���Ļ
        }

        arm_max_q31((q31_t *)adc_value, 320, (q31_t *)&max, &max_index);
        arm_min_q31((q31_t *)adc_value, 320, (q31_t *)&min, &min_index); //��adc������ԭ���ݵ�ǰ320��������Ѱ�����ֵ����Сֵ
        mid = min + (max - min) / 2; //���м�ֵ

        diff_sum = 0;
        for(i=0; i< 320; i++)
        {
            diff_sum += abs((int32_t)(adc_value[i] - mid));
        }
        diff_sum = diff_sum / 320; //����ǰ320����������м�ֵ��ƽ��ƫ��
        
        if(diff_sum <= 2) //�����320����������м�ֵ��ƽ��ƫ��С��2
        {
            freq = 0.0f; //����Ϊ����Ϊֱ���źţ�Ƶ��Ϊ0
        }
        else
        {
            for(i=0; i<1024; i++)
            {
                fft_input[i*2] = (adc_value[i] - ZERO_VALUE) / RESOLU;  //fft��������ǰ1024������ʵ������adcֵת����ʵ��ѹֵ��
                fft_input[i*2+1] = 0;  //fft���������1024�������鲿���鲿ȫΪ0��
            }
            arm_cfft_radix4_f32(&scfft, fft_input);  //���п��ٸ���Ҷ�任

            arm_cmplx_mag_f32(fft_input, fft_output, 1024);  //�������Ҷ�任���Ƶ��

            fft_output[0] = 0.0f;  //����Ƶ������ĵ�1��������Ϊ����ĳ����Ѿ��ж�������źŲ���ֱ���źţ����Կ���ֱ��ȥ��ֱ��������
            arm_max_f32(fft_output, 512, &fft_max_value, &fft_max_index);  //ȡ��Ƶ�������е����ֵ
            freq = 168000000.0f / tim_period / tim_prescaler / 1024.0f * fft_max_index / 1000.0f; //����Ƶ�ʣ�����Ƶ��/��������*���ֵ���±� ������ĵ�λ��HZ��
        }
        
        for(i=0; i<320; i++)  //�������
        {
            adc_value[i] = adc_value[i] > 220 ? 220 : adc_value[i];
            Lcd_Draw_Point(i, 240-adc_value[i], YELLOW);
            Lcd_Draw_Point(i, 17, GREEN);
            Lcd_Draw_Point(i, 167, WHITE);
        }
        max_index = max_index > 3 ? max_index : 3;
        min_index = min_index > 3 ? min_index : 3;

        Lcd_ShowChar(max_index-3, 240-(adc_value[max_index]+6), '*', SHOW_SIZE_16, RED);
        Lcd_ShowChar(min_index-3, 240-(adc_value[min_index]+6), '*', SHOW_SIZE_16, GBLUE);  //��ǳ����ֵ����Сֵ

        max_true = (max - ZERO_VALUE) / RESOLU;
        min_true = (min - ZERO_VALUE) / RESOLU;
        max_true = max_true * VOLTAGE_COEFF;
        min_true = min_true * VOLTAGE_COEFF;  //������ʵ��ѹֵ

        Lcd_Osci_printf("Max:%1.1fV Min:%1.1fV Freq:%2.2fkHz   ", max_true, min_true, freq);  //��ӡ���β�������

        HAL_Delay(1000);
        HAL_ADC_Start_DMA(&hadc1, adc_value, 1024);  //���½�����һ���ڲ���
        HAL_TIM_Base_Start(&htim8);
        adc_get_flag = false;
    } 

    if(Touch_Key_Value != TOUCH_KEY_NONE)
    {
        switch (Touch_Key_Value)
        {
            case TOUCH_KEY_UP:  //�����������ϡ�������������ڣ�һ������10%
                {
                    tim_prescaler += tim_prescaler / 10;
                    __HAL_TIM_SET_PRESCALER(&htim8, tim_prescaler);
                }
            break;
            case TOUCH_KEY_DOWN:  //�����������¡�����С��������һ�μ�С10%
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
