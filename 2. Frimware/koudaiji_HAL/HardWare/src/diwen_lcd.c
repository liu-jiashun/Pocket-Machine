#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "diwen_lcd.h"

Uart_Receive_Struct DiWenLcd_Receive;  //�������������ݵĴ��ڽṹ�����
GPIO_Struct MAX485_Pin; //max485оƬ�������Žṹ�����
UartLcdData mUserUartLcdData = {0};  //�������ṹ�����

void MAX485_Init(void)  //485�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    MAX485_Pin.GPIOx = GPIOB;  MAX485_Pin.GPIO_Pin = GPIO_PIN_15; //B15

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = MAX485_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_InitStruct.Pull = GPIO_PULLDOWN; //��������ʹ��
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; //Ĭ�Ϸ�ת�ٶ�
    HAL_GPIO_Init(MAX485_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

void MAX485_Mode(MAX485_Mode_Enum mode)  //485����ģʽ����
{
	if(mode == MAX485_SEND)
		HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

/*
*���ܣ������������Ҫ��ʾ��������Ϣ
*������line���ڼ��У�str��Ҫ��ʾ�����ݣ�len�����ݳ���
*/
void DiWenLcd_Send_Data(uint8_t line, const char *str, uint8_t len)
{
	uint16_t addr = 0x5000 + line * 0x20;  //�����м����ַ
	uint8_t send_buf[128];
	
	send_buf[0] = 0x5A;
	send_buf[1] = 0xA5;
	send_buf[2] = len+3;
	send_buf[3] = 0x82;
	send_buf[4] = addr>>8;
	send_buf[5] = addr&0xff;  //Э��ͷ����Ϣ
	
	memcpy(&send_buf[6], str, len);  //��Ҫ���͵��ַ�������send_buf��
	
	MAX485_Mode(MAX485_SEND); //MAX485����Ϊ����ģʽ
	HAL_Delay(5);
    HAL_UART_Transmit(&huart5, send_buf, len+6, HAL_MAX_DELAY);  //��������
	HAL_Delay(5);
	MAX485_Mode(MAX485_RECEIVE);  //��MAX485��ת�ɽ���ģʽ
	HAL_Delay(5);
}

/*��������ʼ��*/
void DiWenLcd_Init(void)
{
	uint16_t i;

	mUserUartLcdData.cur_line = 0;
	mUserUartLcdData.cur_page = 0;
	mUserUartLcdData.mem_page = 0;

	for(i=0;i<DIWENLCD_MEM_LINE;i++)  //��������Ŀ�洢�����
	{
		memset(mUserUartLcdData.line_str[i],0x20,32); 
	}
	for(i=0;i<DIWEN_SHOW_LINE;i++) //��������ǰҳ����
	{
		DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[i], 32);
		HAL_Delay(10);
	}

	DiWenLcd_Cmd_Data(0x6000, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x6002, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x6004, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x6006, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x6008, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x6020, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x600A, 0);
	HAL_Delay(10);
	DiWenLcd_Cmd_Data(0x600C, 0);  //������������������ʾ������
	HAL_Delay(10);
	DiWenLcd_Show_RunState(130, "        ");  //������������״̬��ʾ�����
	HAL_Delay(10);
}

void DiWenLcd_Append_Str(const char *str)
{
	uint16_t i;

	memcpy(mUserUartLcdData.line_str[mUserUartLcdData.cur_line],str,strlen(str));
	mUserUartLcdData.mem_page = mUserUartLcdData.cur_line / 17;
	mUserUartLcdData.cur_page = mUserUartLcdData.mem_page;  //���ݵ�ǰ��Ϣ����������ʾҳ��

	if(mUserUartLcdData.cur_line < DIWEN_SHOW_LINE)  //�����ǰС��17�У�ֱ����ʾ
	{
		DiWenLcd_Send_Data(mUserUartLcdData.cur_line, mUserUartLcdData.line_str[mUserUartLcdData.cur_line], 32);
		mUserUartLcdData.cur_line++;
	}
	else  //����17��֮����Ҫ�����ݶ�����һ��Ȼ������ʾ�����һ��
	{
		for(i=0;i<DIWEN_SHOW_LINE;i++)
		{
			DiWenLcd_Send_Data(i, mUserUartLcdData.line_str[mUserUartLcdData.cur_line-16+i], 32);
			HAL_Delay(10);
		}

		if(mUserUartLcdData.cur_line < DIWENLCD_MEM_LINE)
		{
			mUserUartLcdData.cur_line++;
		}
	}
}

/*��������ӡ��Ϣ*/
void DiWenLcd_Printf(const char *fmt, ...)  
{
	char buffer[128];
	va_list arg;
	
	va_start(arg, fmt);
	vsnprintf((char *)buffer, sizeof(buffer),fmt,arg);  //ͨ��C��ɱ�����͸�ʽ����ӡ����Ϣ�浽buffer��
	va_end(arg);
	
	DiWenLcd_Append_Str(buffer);  //���õ�������ӡ���������ݴ�ӡ����Ļ
	HAL_Delay(1);
}

/*��������ʾ����������Ϣ*/
void DiWenLcd_Cmd_Data(uint16_t addr, uint16_t data)
{
    uint8_t buf[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00};

    buf[4] = addr >> 8;
    buf[5] = addr & 0x00ff;
    buf[6] = data >> 8;
    buf[7] = data & 0x00ff;

	MAX485_Mode(MAX485_SEND);
	HAL_Delay(5);
    HAL_UART_Transmit(&huart5, buf, 8, HAL_MAX_DELAY);
	HAL_Delay(5);
	MAX485_Mode(MAX485_RECEIVE);
	HAL_Delay(5);
}

/*��������ʾ��ǰ����״̬*/
void DiWenLcd_Show_RunState(uint16_t addr, const char *fmt, ...)
{
	char str[64] = {0};

	char buffer[128] = {0};
	va_list arg;
	
	va_start(arg, fmt);
	vsnprintf((char *)buffer, sizeof(buffer),fmt,arg);
	va_end(arg);
	
	memset(str,0x20,64);
	DiWenLcd_Send_Data(addr, str, 64);
	HAL_Delay(2);

	memcpy(str,buffer,64);	
	DiWenLcd_Send_Data(addr, str, 64);
	HAL_Delay(2);
}

/*��������ҳ*/
void DiWenLcd_Show_Page(uint8_t page)
{
	uint8_t send_buf[10];
	
	send_buf[0] = 0x5A;
	send_buf[1] = 0xA5;
	send_buf[2] = 0x07;
	send_buf[3] = 0x82;
	send_buf[4] = 0x00;
	send_buf[5] = 0x84;
	send_buf[6] = 0x5A;
	send_buf[7] = 0x01;
	send_buf[8] = 0x00;
	send_buf[9] = page;

	MAX485_Mode(MAX485_SEND);
	HAL_Delay(5);
	HAL_UART_Transmit(&huart5, send_buf, 10, HAL_MAX_DELAY);
	HAL_Delay(5);
	MAX485_Mode(MAX485_RECEIVE);
	HAL_Delay(5);
}

/*
*���ܣ��������������ݽ���
*����ֵ��0��δ���������ݣ�1���������һҳ��������2���������һҳ������
*/
uint8_t DiWen_Dat_Explan(void)
{
	uint8_t buf[9];

	if(!DiWenLcd_Receive.receive_flag)//�жϴ��ڽ����������
	{
		return 0;
	}
	
	DiWenLcd_Receive.receive_flag = 0;//���־
	
	HAL_NVIC_DisableIRQ(UART5_IRQn);
	memcpy(buf, DiWenLcd_Receive.receive_buffer, 9);
	HAL_NVIC_EnableIRQ(UART5_IRQn); //�Ӵ��ڽ��ջ�����ȡ����ʱ��Ҫ�ȹرս����жϣ�������֮���ٴ򿪣�����ȡ���Ĺ����в����ж����ȡ�������ݳ���

	if((buf[0]==0x5A)&&(buf[1]==0xA5))
	{
		if(buf[4]==0x60)
		{
			if(buf[5]==0x62)
			{
				return 2;
			}
			else if(buf[5]==0x60)
			{
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;//���ݲ���
	}
	
	return 0;
}

//5A A5 07 82 00 84 5A 01 00 02 //��2��

// 5A A5 06 83 60 60 01 00 01 //��
// 5A A5 06 83 60 62 01 00 02 //��




