#include "main.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "adc.h"

#include "touch_key.h"
#include "vision.h"
#include "diwen_lcd.h"
#include "tripod.h"
#include "infared.h"
#include "matrix_key.h"
#include "voice.h"
#include "led.h"

#include "run_mode.h"
#include "osci.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_adc1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

extern uint32_t run_time;

extern volatile bool adc_get_flag; 

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
	while (1)
	{
	}
}

void MemManage_Handler(void)
{
	while (1)
	{
	}
}

void BusFault_Handler(void)
{
	while (1)
	{
	}
}

void UsageFault_Handler(void)
{
	while (1)
	{
	}
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)  //ϵͳsystick�жϣ�1msһ��
{
	static uint16_t num;

	HAL_IncTick();
	Touch_Key_Scan();  //��������ɨ��
	Tripod_Key_Scan();  //��̨������λ�źż��
	Matrix_Key_Scan();  //���󰴼�ɨ��
	Voice_Busy_Scan();  //����ģ��æ���

	main_1ms_flag = true;  //������ʱ�����

	num++;
	if(num >= 500)
	{
		num = 0;
		HAL_GPIO_TogglePin(LED0.GPIOx, LED0.GPIO_Pin);  //�����ƣ�ָʾ��̨������������
	}

	if(Run_Mode_Now != &Run_Mode_Stop)
	{
		run_time++;
	}
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

void TIM2_IRQHandler(void)  //��ʱ��2�жϿ�����̨X����
{
	HAL_TIM_IRQHandler(&htim2);

	HAL_GPIO_TogglePin(Tripod_X_STEP.GPIOx, Tripod_X_STEP.GPIO_Pin);  //���������step�����������������
	Tripod_X.step_num--;  //�����������

	if(Tripod_X.step_code <= 84*2)
	{
		if((Tripod_X.step_code - Tripod_X.step_num) < (Tripod_X.step_code/2))
		{
			__HAL_TIM_SET_PRESCALER(&htim2, (84*5) - (Tripod_X.step_code - Tripod_X.step_num)*4);
		}
		else
		{
			__HAL_TIM_SET_PRESCALER(&htim2, (84*5) - (Tripod_X.step_code/2)*4 + (Tripod_X.step_code - Tripod_X.step_num)*4);
		}
	}
	else
	{
		if((Tripod_X.step_code - Tripod_X.step_num) < 84)
		{
			__HAL_TIM_SET_PRESCALER(&htim2, (84*5) - (Tripod_X.step_code - Tripod_X.step_num)*4);
		}
		else if (Tripod_X.step_num < 84)
		{
			__HAL_TIM_SET_PRESCALER(&htim2, 84 + (84 - Tripod_X.step_num)*4);
		}
		
	}

	if(Tripod_X.step_num == 0) //��ָ����������������֮��
	{
		HAL_TIM_Base_Stop_IT(&htim2);  //�رն�ʱ��2
		Tripod_X.is_running = false;  //��̨X���������б�־����
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE); //�����ʱ��2��������жϱ�־
	}
}

void TIM4_IRQHandler(void)  //��ʱ��4�жϿ�����̨Y�������߼��Ͷ�ʱ��2��ͬ��
{
	HAL_TIM_IRQHandler(&htim4);

	HAL_GPIO_TogglePin(Tripod_Y_STEP.GPIOx, Tripod_Y_STEP.GPIO_Pin);
	Tripod_Y.step_num--;

	if(Tripod_Y.step_code <= 84*2)
	{
		if((Tripod_Y.step_code - Tripod_Y.step_num) < (Tripod_Y.step_code/2))
		{
			__HAL_TIM_SET_PRESCALER(&htim4, (84*5) - (Tripod_Y.step_code - Tripod_Y.step_num)*4);
		}
		else
		{
			__HAL_TIM_SET_PRESCALER(&htim4, (84*5) - (Tripod_Y.step_code/2)*4 + (Tripod_Y.step_code - Tripod_Y.step_num)*4);
		}
	}
	else
	{
		if((Tripod_Y.step_code - Tripod_Y.step_num) < 84)
		{
			__HAL_TIM_SET_PRESCALER(&htim4, (84*5) - (Tripod_Y.step_code - Tripod_Y.step_num)*4);
		}
		else if (Tripod_Y.step_num < 84)
		{
			__HAL_TIM_SET_PRESCALER(&htim4, 84 + (84 - Tripod_Y.step_num)*4);
		}
		
	}

	if(Tripod_Y.step_num == 0)
	{
		HAL_TIM_Base_Stop_IT(&htim4);
		Tripod_Y.is_running = false;
		__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
	}
}

void TIM3_IRQHandler(void)  //��ʱ��3�жϣ�38KHz����ģ������źŷ����߼�
{
	HAL_TIM_IRQHandler(&htim3);

	//9ms��   4.5ms����      0��560us��  560us����     1��560us�� 1960us����
	//�����źŷ���38KHZ���ز�ʱ������1���������ز�ʱ������0
	static uint16_t i = 0;
	static uint16_t i_code = 0;
	static uint8_t j;
	i++;
	//������ 9ms�ߣ�4.5ms��
	if(i < 350)
	{
		HAL_GPIO_TogglePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin);
	}
	else if (i == 350)
	{
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
	}
	else if(i == 520)
	{
		i_code = i;
	}
	else if(i > 520)
	{
		//�û���+��ֵ+��ֵ���� Byte 8bit
		if(j < 32)
		{
			if((i > i_code) && (i < (i_code + 22)))  //����560us�ز�
			{
				HAL_GPIO_TogglePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin);
			}
			else if(i == (i_code + 23))  //�������ز�560us
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
			}
			if(i == i_code + 43)
			{
				if(!((Infared.Infared_Data.send_data << j) & 0x80000000)) //�����λ��0������ϣ�i_code�������������һλ
				{
					j++;
					i_code = i;
				}
			}
			if(i == (i_code + 85)) //��������˴�֤����λΪ1���Ѿ�1960us�����͵͵�ƽ
			{
				j++;
				i_code = i;
			}
		}
		else
		{
			if((i > i_code) && (i < (i_code + 22)))  //����ֹͣλ������560us�ز�
			{
				HAL_GPIO_TogglePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin);
			}
			else if(i == (i_code + 22))  //һ֡���ͽ���
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET); //�رպ��ⷢ��ͷ
				HAL_TIM_Base_Stop_IT(&htim3); //�رն�ʱ��3
				__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); //�����ʱ��3�жϱ�־
				Infared.Infared_Is_Busy = false;  //�������æ��־
				i = 0;
				i_code = 0;
				j = 0;
			}
		}
	}
}

void SPI1_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi1);
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);

	if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1); // ��������жϱ�־�������һֱ���Ͻ����жϣ�
		Vision_Receive.receive_flag = true; //��ǽ��յ�һ֡����
		HAL_UART_DMAStop(&huart1); //�رմ���DMA���գ��ȹر�DMA�ٴ򿪣�DMA�ͻ��ͷ��ʼ���գ����ҽ��ռ������㣩
		HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE);  //�ٴδ򿪴���DMA����
	}
}

/**
 * @brief This function handles UART4 global interrupt.
 */
void UART4_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart4);
}

/**
 * @brief This function handles UART5 global interrupt.
 */
void UART5_IRQHandler(void)
{
	if (RESET != __HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE)) // �ж��Ƿ��ǿ����ж�
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart5); // ��������жϱ�־�������һֱ���Ͻ����жϣ�
		huart5.RxState = HAL_UART_STATE_READY;  //��uart5��״̬��Ϊready
		DiWenLcd_Receive.receive_flag = true;  //��ǽ��յ�һ֡����
		HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); //�򿪴����жϽ��գ��������û��uart״̬��Ϊready��˴���ʧ�ܣ�
		uart5_receive_num = 1; //���ݴ��뻺������λ��
	}
	HAL_UART_IRQHandler(&huart5);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  //�������ݽ����ж�
{
	if (huart->Instance == UART5) //����Ǵ���5
	{
		HAL_UART_Receive_IT(&huart5, &DiWenLcd_Receive.receive_buffer[uart5_receive_num++], 1); //�����ݴ��뻺������ָ��λ��
	}
}

void DMA2_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream7_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void DMA2_Stream0_IRQHandler(void)
{
  	HAL_DMA_IRQHandler(&hdma_adc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)  //ADC��������ж�
{
	adc_get_flag = true; //��λADC������ɱ�־λ
}
