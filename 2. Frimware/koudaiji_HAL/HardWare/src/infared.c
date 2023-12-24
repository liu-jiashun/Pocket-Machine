#include "stm32f4xx_hal.h"
#include "infared.h"
#include "tim.h"
#include "beep.h"
#include "lcd.h"
#include "core.h"

GPIO_Struct Infared_Pin; // ����������Žṹ�����
Infared_Struct Infared;	 // ����ṹ�����

/*����������ų�ʼ��*/
void Infared_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	Infared_Pin.GPIOx = GPIOG;
	Infared_Pin.GPIO_Pin = GPIO_PIN_12; // G12

	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = Infared_Pin.GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;		   // �������
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;			   // ����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // ���ŷ�ת�ٶ�Ϊ����
	HAL_GPIO_Init(Infared_Pin.GPIOx, &GPIO_InitStruct);

	HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);

	Infared.Infared_Is_Pause = false;
}

//=====================����ң��==================

// 9ms ��   4.5ms����      0��560us��  560us����     1��560us�� 1960us����

// ����ң�����ķ���   ���ź� ֹͣ  �������  ����ң���������İ���

/*
 *���ܣ�ͨ�����ⷢ������
 *������user1-user2���û��룬data������
 */
void Infared_Send_Data(uint8_t user1, uint8_t user2, uint8_t data)
{
	while (Infared.Infared_Is_Busy); // �������æ�ȴ�
	Infared.Infared_Data.buf[3] = user1;
	Infared.Infared_Data.buf[2] = user2;
	Infared.Infared_Data.buf[1] = data;
	Infared.Infared_Data.buf[0] = ~data; // ���ݷ���

	Infared.Infared_Is_Busy = true; // ��Ǻ���æ
	HAL_TIM_Base_Start_IT(&htim3);	// �򿪶�ʱ��3��ʼ��������
}

/*
 *���ܣ�ͨ�����ⷢ������
 *������user1-user2���û��룬data������
 */
void Infared_Soft_Send_Data(uint8_t user1, uint8_t user2, uint8_t data)
{
	uint16_t i,j;

	Infared.Infared_Data.buf[3] = user1;
	Infared.Infared_Data.buf[2] = user2;
	Infared.Infared_Data.buf[1] = data;
	Infared.Infared_Data.buf[0] = ~data; // ���ݷ���

	HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);

	//__disable_irq();

	for(i=0; i<345; i++) //9ms��
	{
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
		Soft_Delay_Us(13);
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
		Soft_Delay_Us(13);
	}
	Soft_Delay_Us(4500); //4.5ms����

	for(j=0; j<32; j++)
	{
		if(!((Infared.Infared_Data.send_data << j) & 0x80000000)) //�����λ��0
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
	for(i=0; i<22; i++)  //ֹͣλ������560us�ز�
	{
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_SET);
		Soft_Delay_Us(13);
		HAL_GPIO_WritePin(Infared_Pin.GPIOx, Infared_Pin.GPIO_Pin, GPIO_PIN_RESET);
		Soft_Delay_Us(13);
	}

	//__enable_irq();
}

/*ģ��ң�ط���OK��*/
void Infared_Send_OK(void)
{
	Infared_Soft_Send_Data(0xA8, 0x1B, 0x14);
	Beep_Control(BEEP_ON);
	HAL_Delay(10);
	Beep_Control(BEEP_OFF);

	Infared.Infared_Is_Pause = !Infared.Infared_Is_Pause;
}
