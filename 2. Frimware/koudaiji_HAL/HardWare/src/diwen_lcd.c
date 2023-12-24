#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "diwen_lcd.h"

Uart_Receive_Struct DiWenLcd_Receive;  //迪文屏接收数据的串口结构体变量
GPIO_Struct MAX485_Pin; //max485芯片控制引脚结构体变量
UartLcdData mUserUartLcdData = {0};  //迪文屏结构体变量

void MAX485_Init(void)  //485控制引脚初始化
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    MAX485_Pin.GPIOx = GPIOB;  MAX485_Pin.GPIO_Pin = GPIO_PIN_15; //B15

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = MAX485_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLDOWN; //下拉电阻使能
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; //默认翻转速度
    HAL_GPIO_Init(MAX485_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

void MAX485_Mode(MAX485_Mode_Enum mode)  //485工作模式控制
{
	if(mode == MAX485_SEND)
		HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MAX485_Pin.GPIOx, MAX485_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

/*
*功能：向迪文屏发送要显示的文字信息
*参数：line：第几行，str：要显示的数据，len：数据长度
*/
void DiWenLcd_Send_Data(uint8_t line, const char *str, uint8_t len)
{
	uint16_t addr = 0x5000 + line * 0x20;  //根据行计算地址
	uint8_t send_buf[128];
	
	send_buf[0] = 0x5A;
	send_buf[1] = 0xA5;
	send_buf[2] = len+3;
	send_buf[3] = 0x82;
	send_buf[4] = addr>>8;
	send_buf[5] = addr&0xff;  //协议头部信息
	
	memcpy(&send_buf[6], str, len);  //将要发送的字符拷贝进send_buf里
	
	MAX485_Mode(MAX485_SEND); //MAX485设置为发送模式
	HAL_Delay(5);
    HAL_UART_Transmit(&huart5, send_buf, len+6, HAL_MAX_DELAY);  //发送数据
	HAL_Delay(5);
	MAX485_Mode(MAX485_RECEIVE);  //将MAX485再转成接收模式
	HAL_Delay(5);
}

/*迪文屏初始化*/
void DiWenLcd_Init(void)
{
	uint16_t i;

	mUserUartLcdData.cur_line = 0;
	mUserUartLcdData.cur_page = 0;
	mUserUartLcdData.mem_page = 0;

	for(i=0;i<DIWENLCD_MEM_LINE;i++)  //将数据条目存储区清空
	{
		memset(mUserUartLcdData.line_str[i],0x20,32); 
	}
	for(i=0;i<DIWEN_SHOW_LINE;i++) //迪文屏当前页清屏
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
	DiWenLcd_Cmd_Data(0x600C, 0);  //将迪文屏整数数据显示区清零
	HAL_Delay(10);
	DiWenLcd_Show_RunState(130, "        ");  //将迪文屏运行状态显示区清空
	HAL_Delay(10);
}

void DiWenLcd_Append_Str(const char *str)
{
	uint16_t i;

	memcpy(mUserUartLcdData.line_str[mUserUartLcdData.cur_line],str,strlen(str));
	mUserUartLcdData.mem_page = mUserUartLcdData.cur_line / 17;
	mUserUartLcdData.cur_page = mUserUartLcdData.mem_page;  //根据当前信息行数计算显示页数

	if(mUserUartLcdData.cur_line < DIWEN_SHOW_LINE)  //如果当前小于17行，直接显示
	{
		DiWenLcd_Send_Data(mUserUartLcdData.cur_line, mUserUartLcdData.line_str[mUserUartLcdData.cur_line], 32);
		mUserUartLcdData.cur_line++;
	}
	else  //超过17行之后需要将数据都上移一行然后再显示在最后一行
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

/*迪文屏打印信息*/
void DiWenLcd_Printf(const char *fmt, ...)  
{
	char buffer[128];
	va_list arg;
	
	va_start(arg, fmt);
	vsnprintf((char *)buffer, sizeof(buffer),fmt,arg);  //通过C库可变变量和格式化打印将信息存到buffer中
	va_end(arg);
	
	DiWenLcd_Append_Str(buffer);  //调用迪文屏打印函数将数据打印到屏幕
	HAL_Delay(1);
}

/*迪文屏显示整数类型信息*/
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

/*迪文屏显示当前运行状态*/
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

/*迪文屏切页*/
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
*功能：迪文屏返回数据解析
*返回值：0：未解析出数据，1：点击“上一页”按键，2：点击“下一页”按键
*/
uint8_t DiWen_Dat_Explan(void)
{
	uint8_t buf[9];

	if(!DiWenLcd_Receive.receive_flag)//判断串口接收数据完成
	{
		return 0;
	}
	
	DiWenLcd_Receive.receive_flag = 0;//清标志
	
	HAL_NVIC_DisableIRQ(UART5_IRQn);
	memcpy(buf, DiWenLcd_Receive.receive_buffer, 9);
	HAL_NVIC_EnableIRQ(UART5_IRQn); //从串口接收缓冲区取数据时需要先关闭接收中断，接收完之后再打开，避免取数的过程中产生中断造成取出的数据出错

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
		return 0;//数据不对
	}
	
	return 0;
}

//5A A5 07 82 00 84 5A 01 00 02 //切2屏

// 5A A5 06 83 60 60 01 00 01 //上
// 5A A5 06 83 60 62 01 00 02 //下




