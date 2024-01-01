#include <stdlib.h>
#include "main.h"
#include "core.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "adc.h" //оƬ������

#include "led.h"
#include "beep.h"
#include "lcd.h"
#include "touch_key.h" //�ڴ�������

#include "vision.h"
#include "voice.h"
#include "vision.h"
#include "diwen_lcd.h"
#include "infared.h"
#include "tripod.h"
#include "matrix_key.h"
#include "power.h"
#include "relay.h" //����豸

#include "run_mode.h" //�����߼�
#include "logic.h"
#include "osci.h"

volatile bool main_1ms_flag = false; // ������1msʱ������ж�
uint32_t run_time = 0;

uint8_t i;

int main(void)
{
	HAL_Init();						// HAL���ʼ��
	SystemClock_Config(); // ʱ�ӳ�ʼ��
	MX_GPIO_Init();				// GPIO��ʼ��
	Led_Init();						// �ڴ���LED�Ƴ�ʼ��
	Beep_Init();					// �ڴ�����������ʼ��
	Touch_Key_Init();			// �ڴ�������������ʼ��
	HAL_Delay(2000);
	if (Touch_Key_Value == TOUCH_KEY_C) // �����⵽������C������
	{
		Touch_Key_Value = TOUCH_KEY_NONE;
		HAL_Delay(2000);
		if (Touch_Key_Value == TOUCH_KEY_C) // ��ʱ����֮���ܼ�⵽������C������
		{
			Touch_Key_Value = TOUCH_KEY_NONE;

			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			HAL_Delay(200);
			Beep_Control(BEEP_ON);
			HAL_Delay(10);
			Beep_Control(BEEP_OFF);
			MX_DMA_Init();	// DMA��ʼ��
			MX_ADC1_Init(); // ADC1��ʼ��
			MX_TIM8_Init(); // ��ʱ��8��ʼ�������ڴ���ADC���� 800k
			MX_SPI1_Init(); // SPI1��ʼ�����ڴ���lcd��ʹ�ã�
			Lcd_Pin_Init(); // �ڴ���lcd���ӿڳ�ʼ��
			Lcd_Init();			// �ڴ���lcd����ʼ��

			MX_USART1_UART_Init();

			HAL_Delay(100);
			Lcd_Clear(BLACK); // lcd����

			adc_value = malloc(1024 * sizeof(uint32_t)); // �����ڴ棬���ڴ��ADC�ɼ�������

			HAL_ADC_Start_DMA(&hadc1, adc_value, 1024); // ADC1��DMA���գ�һ�δ���1024������
			HAL_TIM_Base_Start(&htim8);									// �򿪶�ʱ��8���������ʱ�ᴥ��ADC���� //800K

			arm_cfft_radix4_init_f32(&scfft, 1024, 0, 1); // ��ʼ��scfft�ṹ�壬�趨fft��ز���

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

			MX_DMA_Init();				 // DMA��ʼ��
			MX_SPI1_Init();				 // SPI1��ʼ�����ڴ���lcd��ʹ�ã�
			MX_USART1_UART_Init(); // ����1��ʼ��������AIʶ��ģ��ʹ�ã�
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
			HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE); // ����1ʹ��DMA���գ�������DMA��������жϺʹ��ڿ����ж�
			MX_UART4_Init();																																// ����4��ʼ������������ģ�飩
			MAX485_Init();																																	// UARTת485оƬ-MAX485��ʼ�������ӵ�������
			MX_UART5_Init();																																// ����5��ʼ����ͨ��MAX485���ӵ�������
			__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(&huart5);
			HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); // ����5���������ݽ����жϺͿ����жϽ�������
			uart5_receive_num = 1;																						// �ƶ����ݷ��봮��5���ջ�������λ�á�
			MX_TIM4_Init();																										// ��ʱ��4��ʼ����������̨Y�Ჽ�����
			MX_TIM3_Init();																										// ��ʱ��3��ʼ�������ƺ���ͷģ��ң���ź� 26us
			MX_TIM2_Init();																										// ��ʱ��2��ʼ����������̨X�Ჽ����� 100us

			Led_Init();				// �ڴ���LED�Ƴ�ʼ��
			Beep_Init();			// �ڴ�����������ʼ��
			Touch_Key_Init(); // �ڴ�������������ʼ��
			Lcd_Pin_Init();		// �ڴ���lcd���ӿڳ�ʼ��
			Lcd_Init();				// �ڴ���lcd����ʼ��

			Power_Init();			 // ��ӹ��ʵ�Դģ���ʼ��
			Matrix_Key_Init(); // ���4*5������ʼ��
			Tripod_Init();		 // ��̨��ʼ��
			HAL_GPIO_WritePin(Tripod_X_EN.GPIOx, Tripod_X_EN.GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Tripod_Y_EN.GPIOx, Tripod_Y_EN.GPIO_Pin, GPIO_PIN_SET);
			Relay_Init();		 // ��Ӽ̵�����ʼ�������Ƽ����ͨ�ϣ�
			Voice_Init();		 // ����ģ���ʼ��
			Infared_Init();	 // ���ⷢ��ͷ��ʼ��
			DiWenLcd_Init(); // ��ӵ�������ʼ��
			HAL_Delay(200);

			DiWenLcd_Show_Page(2); // �����������ڶ�ҳ

			HAL_Delay(200);

			LCD_BACKGROUND = BLUE;
			Lcd_Clear(LCD_BACKGROUND);
			HAL_Delay(10); // LCD��ˢ��

			DiWenLcd_Printf("A:������̨, B:��������");
			DiWenLcd_Printf("C:���Ժ���, D:���Լ����");
			DiWenLcd_Printf("<:����ͨ��,�����л���ɫ��״");
			DiWenLcd_Printf(">:ֹͣ����AIͨ��");

			Matrix_Key_Value = MATRIX_KEY_NONE; // ������󰴼���ֵ
			Touch_Key_Value = TOUCH_KEY_NONE;		// �������������ֵ

			while (1)
			{
				Logic_System_Check();
				HAL_Delay(1);
			}
		}
	}

	MX_DMA_Init();				 // DMA��ʼ��
	MX_SPI1_Init();				 // SPI1��ʼ�����ڴ���lcd��ʹ�ã�
	MX_USART1_UART_Init(); // ����1��ʼ��������AIʶ��ģ��ʹ�ã�
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE); // ����1ʹ��DMA���գ�������DMA��������жϺʹ��ڿ����ж�
	MX_UART4_Init();																																// ����4��ʼ������������ģ�飩
	MAX485_Init();																																	// UARTת485оƬ-MAX485��ʼ�������ӵ�������
	MX_UART5_Init();																																// ����5��ʼ����ͨ��MAX485���ӵ�������
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart5);
	HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); // ����5���������ݽ����жϺͿ����жϽ�������
	uart5_receive_num = 1;																						// �ƶ����ݷ��봮��5���ջ�������λ�á�
	MX_TIM4_Init();																										// ��ʱ��4��ʼ����������̨Y�Ჽ�����
	MX_TIM3_Init();																										// ��ʱ��3��ʼ�������ƺ���ͷģ��ң���ź� 26us
	MX_TIM2_Init();																										// ��ʱ��2��ʼ����������̨X�Ჽ����� 100us

	Led_Init();				// �ڴ���LED�Ƴ�ʼ��
	Beep_Init();			// �ڴ�����������ʼ��
	Touch_Key_Init(); // �ڴ�������������ʼ��
	Lcd_Pin_Init();		// �ڴ���lcd���ӿڳ�ʼ��
	Lcd_Init();				// �ڴ���lcd����ʼ��

	Power_Init();			 // ��ӹ��ʵ�Դģ���ʼ��
	Matrix_Key_Init(); // ���4*5������ʼ��
	Tripod_Init();		 // ��̨��ʼ��
	HAL_GPIO_WritePin(Tripod_X_EN.GPIOx, Tripod_X_EN.GPIO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Tripod_Y_EN.GPIOx, Tripod_Y_EN.GPIO_Pin, GPIO_PIN_SET);
	Relay_Init();		 // ��Ӽ̵�����ʼ�������Ƽ����ͨ�ϣ�
	Voice_Init();		 // ����ģ���ʼ��
	Infared_Init();	 // ���ⷢ��ͷ��ʼ��
	DiWenLcd_Init(); // ��ӵ�������ʼ��

	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF); // ���Ρ�һ������ʾϵͳ��ʼ�����

	LCD_BACKGROUND = RED;
	Lcd_Clear(LCD_BACKGROUND);
	HAL_Delay(10); // LCD��ˢ��

	//	Power_Control(POWER_ON);  //����繦�ʵ�Դ���
	//	HAL_Delay(3000);  //�ȴ���������ȶ�
	Voice_Play((uint8_t)DEVICE_STARTED); // �����������豸��������
	DiWenLcd_Show_Page(2);							 // �����������ڶ�ҳ
	HAL_Delay(1000);
	Lcd_ShowZHChar(0, 0, 0, GREEN);
	Lcd_ShowZHChar(16, 0, 1, BROWN);
	Lcd_ShowZHChar(32, 0, 2, BLUE);
	Lcd_ShowString(48, 0, ": 666", SHOW_SIZE_16, BLACK);
	DiWenLcd_Printf("���ڸ�λ");
	Lcd_printf("self resetting");

	Tripod_X_Go_Mechzero(); // ��̨X���е����
	Tripod_Y_Go_Mechzero(); // ��̨Y���е����

	Voice_Play((uint8_t)DEVICE_CHECKED); // �����������Լ���ɡ�

	DiWenLcd_Printf("��λ���");
	Lcd_printf("Reset complete");
	Relay_Control(RELAY_ON); // �򿪼����

	Run_Mode_Init(); // ��ʼ������ģʽ

	Vision_Send_Cmd(select_color, select_shape);
	DiWenLcd_Show_RunState(130, "%s%s", ColorStrZh[select_color - 1], ShapeStrZh[select_shape - 1]);
	HAL_Delay(10);
	DiWenLcd_Show_RunState(129, "ֹͣ���");

	Matrix_Key_Value = MATRIX_KEY_NONE; // ������󰴼���ֵ
	Touch_Key_Value = TOUCH_KEY_NONE;		// �������������ֵ
	run_time = 0;

	while (!manual_claib_finish) // �ȴ��ֶ�У׼���
	{
		if (main_1ms_flag == true) // ��������ʱ��1msһ��
		{
			Logic_Tripod_Calib(); // �ֶ�У׼��̨������ͷ
			main_1ms_flag = false;
		}
	}

	Tripod_X_Coord(684);
	Tripod_Y_Coord(Y_OFFSET);
	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF); // ���Ρ�һ����ʾ�豸��ʼ����

	while (1)
	{
		Logic_Matrix_Key(); // ���󰴼��߼�����
		Logic_Touch_Key();	// ���������߼�����

		if (main_1ms_flag == true) // ��������ʱ��1msһ��
		{
			static uint16_t num;

			Run_Mode_Now->mode_run(); // ���е�ǰģʽ

			num++;
			if (num >= 200)
			{
				num = 0;
				HAL_GPIO_TogglePin(LED1.GPIOx, LED1.GPIO_Pin); // ������ָʾǰ̨������������
			}

			main_1ms_flag = false;
		}

		if (run_time % 1000 == 0) // ÿ1�������������һ������ʱ�����
		{
			DiWenLcd_Cmd_Data(0x6000, run_time / 1000);
			run_time++;
		}

		if (Run_Mode_Now == &Run_Mode_Stop) // ��ͣʱ���ܷ�ҳ
		{
			uint16_t i = 0;
			uint8_t lcd_rece_data;

			lcd_rece_data = DiWen_Dat_Explan(); // ��������������������
			if (lcd_rece_data == 1)							// ���������ˡ���һҳ����
			{
				if (mUserUartLcdData.cur_page > 0)
				{
					mUserUartLcdData.cur_page--; // ��ʾ�洢����һҳ��Ϣ
					for (i = 0; i < DIWEN_SHOW_LINE; i++)
					{
						DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[mUserUartLcdData.cur_page * 17 + i], 32);
						HAL_Delay(10);
					}
				}
				if (mUserLcdData.cur_page > 0)
				{
					mUserLcdData.cur_page--; // ��ʾ�洢����һҳ��Ϣ
					Lcd_Clear_Print(LCD_BACKGROUND);
					for (i = 0; i < 10; i++)
					{
						Lcd_ShowString(0, (i + 4) * 16, mUserLcdData.line_str[mUserLcdData.cur_page * 10 + i], SHOW_SIZE_16, LCD_TYPEFACE);
						HAL_Delay(10);
					}
				}
			}
			else if (lcd_rece_data == 2) // ���������ˡ���һҳ����
			{
				if (mUserUartLcdData.cur_page < mUserUartLcdData.mem_page)
				{
					mUserUartLcdData.cur_page++; // ��ʾ�洢����һҳ��Ϣ
					for (i = 0; i < 17; i++)
					{
						DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[mUserUartLcdData.cur_page * 17 + i], 32);
						HAL_Delay(10);
					}
				}
				if (mUserLcdData.cur_page < mUserLcdData.mem_page)
				{
					mUserLcdData.cur_page++; // ��ʾ�洢����һҳ��Ϣ
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
