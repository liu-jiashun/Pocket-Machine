#include "vision.h"
#include "run_mode.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

Uart_Receive_Struct Vision_Receive;  //����AIģ�����ݵĴ��ڽṹ�����
Vision_Coord_Struct Vision_Coord;  //���յ���������Ϣ�Ľṹ�����

/*
*���ܣ���AIģ�鷢��ָ����ʲô��ɫ����״������
*������color��������ɫ��shape��������״
*/
void Vision_Send_Cmd(uint8_t color, uint8_t shape)
{
	static uint8_t data[8];
	//if(color>4||shape>5)return;
	
	data[0] = 0x5A;
	data[1] = 0xA5;
	data[2] = 0x03;
	data[3] = 0x01;
	data[4] = color;
	data[5] = shape;
	data[6] = 0x9F;
	data[7] = 0xF9;
	
	HAL_UART_Transmit_DMA(&huart1, data, 8);
}

/*
*���ܣ�����AIģ�鷢���������ݰ�
*����ֵ�����ݰ��а����ļ�⵽�����������
*/
uint8_t Vision_Dat_Explan(void)
{
	uint8_t result = 0;
	uint8_t i = 0;
	uint8_t* buf;

	if(!Vision_Receive.receive_flag)//�жϴ��ڽ����������
	{
		return 0;
	}
	Vision_Receive.receive_flag = 0;//���־

	buf = malloc(UART_BUFFER_SIZE);
	if(buf == NULL) return 0;
	
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	memcpy(buf, Vision_Receive.receive_buffer, UART_BUFFER_SIZE);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	if((*(buf+0)==0x5A)&&(*(buf+1)==0xA5))
	{
		result = *(buf+3);
		Vision_Coord.Res_Num = result;
		for(i=0; i<result; i++)
		{
			Vision_Coord.Res_Coord[i].x = (*(buf+4+(i*4)))<<8 | *(buf+5+(i*4));
			Vision_Coord.Res_Coord[i].y = (*(buf+6+(i*4)))<<8 | *(buf+7+(i*4));
		}
		if(result > 1)
		{
//			if(Run_Mode_Now == &Run_Mode_Slow)
//			{
//                Res_Sort2(&Vision_Coord);
//			}
//			else
//			{
				Res_Sort(&Vision_Coord);
//			}
        }
    }
	else
	{
		result = 0;//���ݲ���
	}
	free(buf);

	return result;
}

/*�Լ�⵽�����尴����������Ҵ��ϵ��½�������*/
static void Res_Sort(Vision_Coord_Struct_p wait_sort)
{
	uint8_t i, j;
	uint16_t mad;

	for(i=0; i<wait_sort->Res_Num; i++)  //�����Ҵ��ϵ�������
	{
		for(j=i+1; j<wait_sort->Res_Num; j++)
		{
			if(wait_sort->Res_Coord[j].x < wait_sort->Res_Coord[i].x)
			{
				mad = wait_sort->Res_Coord[i].x;
				wait_sort->Res_Coord[i].x = wait_sort->Res_Coord[j].x;
				wait_sort->Res_Coord[j].x = mad;
				mad = wait_sort->Res_Coord[i].y;
				wait_sort->Res_Coord[i].y = wait_sort->Res_Coord[j].y;
				wait_sort->Res_Coord[j].y = mad;
			}
			if(abs(wait_sort->Res_Coord[j].x - wait_sort->Res_Coord[i].x) < 10)
			{
				if(wait_sort->Res_Coord[j].y > wait_sort->Res_Coord[i].y)
				{
					mad = wait_sort->Res_Coord[i].x;
					wait_sort->Res_Coord[i].x = wait_sort->Res_Coord[j].x;
					wait_sort->Res_Coord[j].x = mad;
					mad = wait_sort->Res_Coord[i].y;
					wait_sort->Res_Coord[i].y = wait_sort->Res_Coord[j].y;
					wait_sort->Res_Coord[j].y = mad;
				}
			}
		}
	}
}

/*�Լ�⵽�����尴��������µ��ϴ����ҽ�������*/
static void Res_Sort2(Vision_Coord_Struct_p wait_sort)
{
	uint8_t i, j;
	uint16_t mad;

	for(i=0; i<wait_sort->Res_Num; i++)
	{
		for(j=i+1; j<wait_sort->Res_Num; j++)
		{
			if(wait_sort->Res_Coord[j].y < wait_sort->Res_Coord[i].y)
			{
				mad = wait_sort->Res_Coord[i].x;
				wait_sort->Res_Coord[i].x = wait_sort->Res_Coord[j].x;
				wait_sort->Res_Coord[j].x = mad;
				mad = wait_sort->Res_Coord[i].y;
				wait_sort->Res_Coord[i].y = wait_sort->Res_Coord[j].y;
				wait_sort->Res_Coord[j].y = mad;
			}
			if(abs(wait_sort->Res_Coord[j].y - wait_sort->Res_Coord[i].y) < 10)
			{
				if(wait_sort->Res_Coord[j].x < wait_sort->Res_Coord[i].x)
				{
					mad = wait_sort->Res_Coord[i].x;
					wait_sort->Res_Coord[i].x = wait_sort->Res_Coord[j].x;
					wait_sort->Res_Coord[j].x = mad;
					mad = wait_sort->Res_Coord[i].y;
					wait_sort->Res_Coord[i].y = wait_sort->Res_Coord[j].y;
					wait_sort->Res_Coord[j].y = mad;
				}
			}
		}
	}
}



