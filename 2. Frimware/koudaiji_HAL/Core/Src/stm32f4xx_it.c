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

void SysTick_Handler(void)  //系统systick中断，1ms一次
{
	static uint16_t num;

	HAL_IncTick();
	Touch_Key_Scan();  //触摸按键扫描
	Tripod_Key_Scan();  //云台电子限位信号检测
	Matrix_Key_Scan();  //矩阵按键扫描
	Voice_Busy_Scan();  //语音模块忙检测

	main_1ms_flag = true;  //主函数时序控制

	num++;
	if(num >= 500)
	{
		num = 0;
		HAL_GPIO_TogglePin(LED0.GPIOx, LED0.GPIO_Pin);  //呼吸灯，指示后台程序运行正常
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

void TIM2_IRQHandler(void)  //定时器2中断控制云台X轴电机
{
	HAL_TIM_IRQHandler(&htim2);

	HAL_GPIO_TogglePin(Tripod_X_STEP.GPIOx, Tripod_X_STEP.GPIO_Pin);  //在驱动板的step引脚上输出控制脉冲
	Tripod_X.step_num--;  //脉冲个数控制

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

	if(Tripod_X.step_num == 0) //当指定个数脉冲输出完成之后
	{
		HAL_TIM_Base_Stop_IT(&htim2);  //关闭定时器2
		Tripod_X.is_running = false;  //云台X轴正在运行标志清零
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE); //清除定时器2计数溢出中断标志
	}
}

void TIM4_IRQHandler(void)  //定时器4中断控制云台Y轴电机（逻辑和定时器2相同）
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

void TIM3_IRQHandler(void)  //定时器3中断（38KHz），模拟红外信号发射逻辑
{
	HAL_TIM_IRQHandler(&htim3);

	//9ms发   4.5ms不发      0：560us发  560us不发     1：560us发 1960us不发
	//红外信号发射38KHZ的载波时代表发送1，不发送载波时代表发送0
	static uint16_t i = 0;
	static uint16_t i_code = 0;
	static uint8_t j;
	i++;
	//引导码 9ms高，4.5ms低
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
		//用户码+键值+键值反码 Byte 8bit
		if(j < 32)
		{
			if((i > i_code) && (i < (i_code + 22)))  //发送560us载波
			{
				HAL_GPIO_TogglePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin);
			}
			else if(i == (i_code + 23))  //不发送载波560us
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
			}
			if(i == i_code + 43)
			{
				if(!((Infared.Infared_Data.send_data << j) & 0x80000000)) //如果该位是0发送完毕，i_code清零继续发送下一位
				{
					j++;
					i_code = i;
				}
			}
			if(i == (i_code + 85)) //函数到达此处证明该位为1，已经1960us不发送低电平
			{
				j++;
				i_code = i;
			}
		}
		else
		{
			if((i > i_code) && (i < (i_code + 22)))  //发送停止位，发送560us载波
			{
				HAL_GPIO_TogglePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin);
			}
			else if(i == (i_code + 22))  //一帧发送结束
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET); //关闭红外发射头
				HAL_TIM_Base_Stop_IT(&htim3); //关闭定时器3
				__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); //清除定时器3中断标志
				Infared.Infared_Is_Busy = false;  //清除红外忙标志
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

	if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) // 判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1); // 清楚空闲中断标志（否则会一直不断进入中断）
		Vision_Receive.receive_flag = true; //标记接收到一帧数据
		HAL_UART_DMAStop(&huart1); //关闭串口DMA接收（先关闭DMA再打开，DMA就会从头开始接收，并且接收计数清零）
		HAL_UART_Receive_DMA(&huart1, Vision_Receive.receive_buffer, UART_BUFFER_SIZE);  //再次打开串口DMA接收
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
	if (RESET != __HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE)) // 判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart5); // 清楚空闲中断标志（否则会一直不断进入中断）
		huart5.RxState = HAL_UART_STATE_READY;  //将uart5的状态置为ready
		DiWenLcd_Receive.receive_flag = true;  //标记接收到一帧数据
		HAL_UART_Receive_IT(&huart5, DiWenLcd_Receive.receive_buffer, 1); //打开串口中断接收（如果上面没将uart状态置为ready则此处打开失败）
		uart5_receive_num = 1; //数据存入缓冲区的位置
	}
	HAL_UART_IRQHandler(&huart5);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  //串口数据接收中断
{
	if (huart->Instance == UART5) //如果是串口5
	{
		HAL_UART_Receive_IT(&huart5, &DiWenLcd_Receive.receive_buffer[uart5_receive_num++], 1); //将数据存入缓冲区的指定位置
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)  //ADC采样完成中断
{
	adc_get_flag = true; //置位ADC采样完成标志位
}
