#include "stm32f4xx_hal.h"
#include "infared.h"
#include "tim.h"
#include "beep.h"
#include "lcd.h"
#include "core.h"

GPIO_Struct Infared_Pin; // 红外控制引脚结构体变量
Infared_Struct Infared;	 // 红外结构体变量

/*红外控制引脚初始化*/
void Infared_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	Infared_Pin.GPIOx = GPIOG;
	Infared_Pin.GPIO_Pin = GPIO_PIN_12; // G12

	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = Infared_Pin.GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		   // 推挽输出
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;			   // 下拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 引脚翻转速度为高速
	HAL_GPIO_Init(Infared_Pin.GPIOx, &GPIO_InitStruct);

	HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);

	Infared.Infared_Is_Pause = false;
}

//=====================红外遥控==================

// 9ms 发   4.5ms不发      0：560us发  560us不发     1：560us发 1960us不发

// 红外遥控器的发送   播放和 停止  都是这个  就是遥控器的中心按键

/*
 *功能：通过红外发送数据
 *参数：user1-user2：用户码，data：数据
 */
void Infared_Send_Data(uint8_t user1, uint8_t user2, uint8_t data)
{
	while (Infared.Infared_Is_Busy); // 如果红外忙等待
	Infared.Infared_Data.buf[3] = user1;
	Infared.Infared_Data.buf[2] = user2;
	Infared.Infared_Data.buf[1] = data;
	Infared.Infared_Data.buf[0] = ~data; // 数据反码

	Infared.Infared_Is_Busy = true; // 标记红外忙
	HAL_TIM_Base_Start_IT(&htim3);	// 打开定时器3开始发送数据
}

/*
 *功能：通过红外发送数据
 *参数：user1-user2：用户码，data：数据
 */
void Infared_Soft_Send_Data(uint8_t user1, uint8_t user2, uint8_t data)
{
	uint16_t i,j;

	Infared.Infared_Data.buf[3] = user1;
	Infared.Infared_Data.buf[2] = user2;
	Infared.Infared_Data.buf[1] = data;
	Infared.Infared_Data.buf[0] = ~data; // 数据反码

	HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);

	//__disable_irq();

	for(i=0; i<345; i++) //9ms发
	{
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
		Soft_Delay_Us(13);
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
		Soft_Delay_Us(13);
	}
	Soft_Delay_Us(4500); //4.5ms不发

	for(j=0; j<32; j++)
	{
		if(!((Infared.Infared_Data.send_data << j) & 0x80000000)) //如果该位是0
		{
			for(i=0; i<24; i++)
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
				Soft_Delay_Us(13);
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
				Soft_Delay_Us(13);
			}
			Soft_Delay_Us(535);
		}
		else
		{
			for(i=0; i<24; i++)
			{
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
				Soft_Delay_Us(13);
				HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
				Soft_Delay_Us(13);
			}
			Soft_Delay_Us(1630);
		}
	}
	for(i=0; i<22; i++)  //停止位，发送560us载波
	{
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
		Soft_Delay_Us(13);
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
		Soft_Delay_Us(13);
	}

	//__enable_irq();
}

/*模拟遥控发送OK键*/
void Infared_Send_OK(void)
{
	Infared_Soft_Send_Data(0xA8, 0x1B, 0x14);
	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF);

	Infared.Infared_Is_Pause = !Infared.Infared_Is_Pause;
}
