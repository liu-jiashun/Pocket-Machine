#include <stdlib.h>
#include "main.h"
#include "core.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "adc.h" //芯片级外设

#include "led.h"
#include "beep.h"
#include "lcd.h"
#include "touch_key.h" //口袋机外设

#include "vision.h"
#include "voice.h"
#include "vision.h"
#include "diwen_lcd.h"
#include "infared.h"
#include "tripod.h"
#include "matrix_key.h"
#include "power.h"
#include "relay.h" //外接设备

#include "run_mode.h" //运行逻辑
#include "logic.h"
#include "osci.h"

volatile bool main_1ms_flag = false; // 主函数1ms时序控制中断
uint32_t run_time = 0;

uint8_t i;

int main(void)
{
	HAL_Init();						// HAL库初始化
	SystemClock_Config(); // 时钟初始化
	MX_GPIO_Init();				// GPIO初始化
	Led_Init();						// 口袋机LED灯初始化
	Beep_Init();					// 口袋机蜂鸣器初始化
	Touch_Key_Init();			// 口袋机触摸按键初始化
	HAL_Delay(2000);
	if (Touch_Key_Value == TOUCH_KEY_C) // 如果检测到按键“C”按下
	{
		Touch_Key_Value = TOUCH_KEY_NONE;
		HAL_Delay(2000);
		if (Touch_Key_Value == TOUCH_KEY_C) // 延时两秒之后还能检测到按键“C”按下
		{
			Touch_Key_Value = TOUCH_KEY_NONE;

			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			HAL_Delay(200);
			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			MX_DMA_Init();	// DMA初始化
			MX_ADC1_Init(); // ADC1初始化
			MX_TIM8_Init(); // 定时器8初始化，用于触发ADC采样 800k
			MX_SPI1_Init(); // SPI1初始化（口袋机lcd屏使用）
			Lcd_Pin_Init(); // 口袋机lcd屏接口初始化
			Lcd_Init();			// 口袋机lcd屏初始化

			MX_USART1_UART_Init();

			HAL_Delay(100);
			Lcd_Clear(BLACK); // lcd清屏

			adc_value = malloc(1024 * sizeof(uint32_t)); // 申请内存，用于存放ADC采集的数据

			HAL_ADC_Start_DMA(&hadc1, adc_value, 1024); // ADC1打开DMA接收，一次传输1024个数据
			HAL_TIM_Base_Start(&htim8);									// 打开定时器8，计数溢出时会触发ADC采样 //800K

			arm_cfft_radix4_init_f32(&scfft, 1024, 0, 1); // 初始化scfft结构体，设定fft相关参数

			while (1)
			{
				Osci_Show();
			}
		}
	}

	if (Touch_Key_Value == TOUCH_KEY_D)
	{
		Touch_Key_Value = TOUCH_KEY_NONE;
		HAL_Delay(2000);
		if (Touch_Key_Value == TOUCH_KEY_D)
		{
			Touch_Key_Value = TOUCH_KEY_NONE;

			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			HAL_Delay(200);
			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			HAL_Delay(200);
			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);

			MX_DMA_Init();				 // DMA初始化
			MX_SPI1_Init();				 // SPI1初始化（口袋机lcd屏使用）
			MX_USART1_UART_Init(); // 串口1初始化（连接AI识别模组使用）
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
			HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE); // 串口1使用DMA接收，开启了DMA接收完成中断和串口空闲中断
			MX_UART4_Init();																																// 串口4初始化（连接语音模块）
			MAX485_Init();																																	// UART转485芯片-MAX485初始化（连接迪文屏）
			MX_UART5_Init();																																// 串口5初始化（通过MAX485连接迪文屏）
			__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(&huart5);
			HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); // 串口5开启了数据接收中断和空闲中断接收数据
			uart5_receive_num = 1;																						// 移动数据放入串口5接收缓冲区的位置。
			MX_TIM4_Init();																										// 定时器4初始化，控制云台Y轴步进电机
			MX_TIM3_Init();																										// 定时器3初始化，控制红外头模拟遥控信号 26us
			MX_TIM2_Init();																										// 定时器2初始化，控制云台X轴步进电机 100us

			Led_Init();				// 口袋机LED灯初始化
			Beep_Init();			// 口袋机蜂鸣器初始化
			Touch_Key_Init(); // 口袋机触摸按键初始化
			Lcd_Pin_Init();		// 口袋机lcd屏接口初始化
			Lcd_Init();				// 口袋机lcd屏初始化

			Power_Init();			 // 外接功率电源模块初始化
			Matrix_Key_Init(); // 外接4*5按键初始化
			Tripod_Init();		 // 云台初始化
			HAL_GPIO_WritePin(Tripod_X_EN.GPIOx, Tripod_X_EN.GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Tripod_Y_EN.GPIOx, Tripod_Y_EN.GPIO_Pin, GPIO_PIN_SET);
			Relay_Init();		 // 外接继电器初始化（控制激光笔通断）
			Voice_Init();		 // 语音模块初始化
			Infared_Init();	 // 红外发射头初始化
			DiWenLcd_Init(); // 外接迪文屏初始化
			HAL_Delay(200);

			DiWenLcd_Show_Page(2); // 迪文屏翻到第二页

			HAL_Delay(200);

			LCD_BACKGROUND = BLUE;
			Lcd_Clear(LCD_BACKGROUND);
			HAL_Delay(10); // LCD屏刷屏

			DiWenLcd_Printf("A:测试云台, B:测试语音");
			DiWenLcd_Printf("C:测试红外, D:测试激光笔");
			DiWenLcd_Printf("<:测试通信,按下切换颜色形状");
			DiWenLcd_Printf(">:停止测试AI通信");

			Matrix_Key_Value = MATRIX_KEY_NONE; // 清除矩阵按键键值
			Touch_Key_Value = TOUCH_KEY_NONE;		// 清除触摸按键键值

			while (1)
			{
				Logic_System_Check();
				HAL_Delay(1);
			}
		}
	}

	MX_DMA_Init();				 // DMA初始化
	MX_SPI1_Init();				 // SPI1初始化（口袋机lcd屏使用）
	MX_USART1_UART_Init(); // 串口1初始化（连接AI识别模组使用）
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE); // 串口1使用DMA接收，开启了DMA接收完成中断和串口空闲中断
	MX_UART4_Init();																																// 串口4初始化（连接语音模块）
	MAX485_Init();																																	// UART转485芯片-MAX485初始化（连接迪文屏）
	MX_UART5_Init();																																// 串口5初始化（通过MAX485连接迪文屏）
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart5);
	HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); // 串口5开启了数据接收中断和空闲中断接收数据
	uart5_receive_num = 1;																						// 移动数据放入串口5接收缓冲区的位置。
	MX_TIM4_Init();																										// 定时器4初始化，控制云台Y轴步进电机
	MX_TIM3_Init();																										// 定时器3初始化，控制红外头模拟遥控信号 26us
	MX_TIM2_Init();																										// 定时器2初始化，控制云台X轴步进电机 100us

	Led_Init();				// 口袋机LED灯初始化
	Beep_Init();			// 口袋机蜂鸣器初始化
	Touch_Key_Init(); // 口袋机触摸按键初始化
	Lcd_Pin_Init();		// 口袋机lcd屏接口初始化
	Lcd_Init();				// 口袋机lcd屏初始化

	Power_Init();			 // 外接功率电源模块初始化
	Matrix_Key_Init(); // 外接4*5按键初始化
	Tripod_Init();		 // 云台初始化
	HAL_GPIO_WritePin(Tripod_X_EN.GPIOx, Tripod_X_EN.GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Tripod_Y_EN.GPIOx, Tripod_Y_EN.GPIO_Pin, GPIO_PIN_SET);
	Relay_Init();		 // 外接继电器初始化（控制激光笔通断）
	Voice_Init();		 // 语音模块初始化
	Infared_Init();	 // 红外发射头初始化
	DiWenLcd_Init(); // 外接迪文屏初始化

	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF); // “滴”一声，提示系统初始化完成

	LCD_BACKGROUND = RED;
	Lcd_Clear(LCD_BACKGROUND);
	HAL_Delay(10); // LCD屏刷屏

	//	Power_Control(POWER_ON);  //打开外界功率电源输出
	//	HAL_Delay(3000);  //等待功率输出稳定
	Voice_Play((uint8_t)DEVICE_STARTED); // 语音播报“设备已启动”
	DiWenLcd_Show_Page(2);							 // 迪文屏翻到第二页
	HAL_Delay(1000);
	Lcd_ShowZHChar(0, 0, 0, GREEN);
	Lcd_ShowZHChar(16, 0, 1, BROWN);
	Lcd_ShowZHChar(32, 0, 2, BLUE);
	Lcd_ShowString(48, 0, ": 666", SHOW_SIZE_16, BLACK);
	DiWenLcd_Printf("正在复位");
	Lcd_printf("self resetting");

	Tripod_X_Go_Mechzero(); // 云台X轴机械归零
	Tripod_Y_Go_Mechzero(); // 云台Y轴机械归零

	Voice_Play((uint8_t)DEVICE_CHECKED); // 语音播报“自检完成”

	DiWenLcd_Printf("复位完成");
	Lcd_printf("Reset complete");
	Relay_Control(RELAY_ON); // 打开激光笔

	Run_Mode_Init(); // 初始化运行模式

	Vision_Send_Cmd(select_color, select_shape);
	DiWenLcd_Show_RunState(130, "%s%s", ColorStrZh[select_color - 1], ShapeStrZh[select_shape - 1]);
	HAL_Delay(10);
	DiWenLcd_Show_RunState(129, "停止检测");

	Matrix_Key_Value = MATRIX_KEY_NONE; // 清除矩阵按键键值
	Touch_Key_Value = TOUCH_KEY_NONE;		// 清除触摸按键键值
	run_time = 0;

	while (!manual_claib_finish) // 等待手动校准完成
	{
		if (main_1ms_flag == true) // 控制运行时序1ms一次
		{
			Logic_Tripod_Calib(); // 手动校准云台和摄像头
			main_1ms_flag = false;
		}
	}

	Tripod_X_Coord(684);
	Tripod_Y_Coord(Y_OFFSET);
	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF); // “滴”一声提示设备开始运行

	while (1)
	{
		Logic_Matrix_Key(); // 矩阵按键逻辑处理
		Logic_Touch_Key();	// 触摸按键逻辑处理

		if (main_1ms_flag == true) // 控制运行时序1ms一次
		{
			static uint16_t num;

			Run_Mode_Now->mode_run(); // 运行当前模式

			num++;
			if (num >= 200)
			{
				num = 0;
				HAL_GPIO_TogglePin(LED1.GPIOx, LED1.GPIO_Pin); // 呼吸灯指示前台程序运行正常
			}

			main_1ms_flag = false;
		}

		if (run_time % 1000 == 0) // 每1秒向迪文屏发送一次运行时间计数
		{
			DiWenLcd_Cmd_Data(0x6000, run_time / 1000);
			run_time++;
		}

		if (Run_Mode_Now == &Run_Mode_Stop) // 暂停时才能翻页
		{
			uint16_t i = 0;
			uint8_t lcd_rece_data;

			lcd_rece_data = DiWen_Dat_Explan(); // 解析迪文屏发出的数据
			if (lcd_rece_data == 1)							// 迪文屏按了“上一页”键
			{
				if (mUserUartLcdData.cur_page > 0)
				{
					mUserUartLcdData.cur_page--; // 显示存储的上一页信息
					for (i = 0; i < DIWEN_SHOW_LINE; i++)
					{
						DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[mUserUartLcdData.cur_page * 17 + i], 32);
						HAL_Delay(10);
					}
				}
				if (mUserLcdData.cur_page > 0)
				{
					mUserLcdData.cur_page--; // 显示存储的上一页信息
					Lcd_Clear_Print(LCD_BACKGROUND);
					for (i = 0; i < 10; i++)
					{
						Lcd_ShowString(0, (i + 4) * 16, mUserLcdData.line_str[mUserLcdData.cur_page * 10 + i], SHOW_SIZE_16, LCD_TYPEFACE);
						HAL_Delay(10);
					}
				}
			}
			else if (lcd_rece_data == 2) // 迪文屏按了“下一页”键
			{
				if (mUserUartLcdData.cur_page < mUserUartLcdData.mem_page)
				{
					mUserUartLcdData.cur_page++; // 显示存储的下一页信息
					for (i = 0; i < 17; i++)
					{
						DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[mUserUartLcdData.cur_page * 17 + i], 32);
						HAL_Delay(10);
					}
				}
				if (mUserLcdData.cur_page < mUserLcdData.mem_page)
				{
					mUserLcdData.cur_page++; // 显示存储的下一页信息
					Lcd_Clear_Print(LCD_BACKGROUND);
					for (i = 0; i < 10; i++)
					{
						Lcd_ShowString(0, (i + 4) * 16, mUserLcdData.line_str[mUserLcdData.cur_page * 10 + i], SHOW_SIZE_16, LCD_TYPEFACE);
						HAL_Delay(10);
					}
				}
			}
		}
	}
}
