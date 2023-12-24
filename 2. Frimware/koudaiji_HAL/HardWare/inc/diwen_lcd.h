#ifndef __DIWEN_LCD_H__
#define __DIWEN_LCD_H__

#include "usart.h"
#include "gpio.h"

#define DIWEN_SHOW_LINE 17  //宏定义迪文屏数据显示行数
#define DIWENLCD_MEM_LINE 200  //宏定义迪文屏数据条目存储深度

typedef struct  //迪文屏相关
{
	char line_str[DIWENLCD_MEM_LINE][32];  //迪文屏数据条目存储区
	uint16_t cur_line;  //当前显示行
	uint16_t cur_page;  //当前显示页数
    uint16_t mem_page;  //记录总页数
}UartLcdData,*pUartLcdData;

typedef enum
{
	MAX485_SEND = 0, //发送模式
	MAX485_RECEIVE,  //接收模式
}MAX485_Mode_Enum;  //MAX458工作模式枚举

extern Uart_Receive_Struct DiWenLcd_Receive;
extern GPIO_Struct MAX485_Pin;
extern UartLcdData mUserUartLcdData;

void MAX485_Init(void);
void MAX485_Mode(MAX485_Mode_Enum mode);
void DiWenLcd_Init(void);
void DiWenLcd_Append_Str(const char *str);
void DiWenLcd_Printf(const char *fmt, ...);

void DiWenLcd_Cmd_Data(uint16_t addr, uint16_t data);
void DiWenLcd_Show_RunState(uint16_t addr, const char *fmt, ...);
void DiWenLcd_Send_Data(uint8_t line, const char *str, uint8_t len);

void DiWenLcd_Show_Page(uint8_t page);
uint8_t DiWen_Dat_Explan(void);

#endif


