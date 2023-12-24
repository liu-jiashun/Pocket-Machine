#ifndef __DIWEN_LCD_H__
#define __DIWEN_LCD_H__

#include "usart.h"
#include "gpio.h"

#define DIWEN_SHOW_LINE 17  //�궨�������������ʾ����
#define DIWENLCD_MEM_LINE 200  //�궨�������������Ŀ�洢���

typedef struct  //���������
{
	char line_str[DIWENLCD_MEM_LINE][32];  //������������Ŀ�洢��
	uint16_t cur_line;  //��ǰ��ʾ��
	uint16_t cur_page;  //��ǰ��ʾҳ��
    uint16_t mem_page;  //��¼��ҳ��
}UartLcdData,*pUartLcdData;

typedef enum
{
	MAX485_SEND = 0, //����ģʽ
	MAX485_RECEIVE,  //����ģʽ
}MAX485_Mode_Enum;  //MAX458����ģʽö��

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


