#ifndef __LCD_H__
#define __LCD_H__

#include "gpio.h"
#include "sys_font.h"

#define LCD_W_SIZE 320
#define LCD_H_SIZE 240  //�궨��ڴ���lcd���ֱ���

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define GREEN         	 0x07E0
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40

extern uint16_t LCD_BACKGROUND; //����ɫ
extern uint16_t LCD_TYPEFACE;  //����ɫ

#define LCD_SHOW_LINE 17  //�궨��LCD��������ʾ����
#define LCD_MEM_LINE 200  //�궨��LCD��������Ŀ�洢���

typedef struct  //���������
{
	char line_str[LCD_MEM_LINE][41];  //������������Ŀ�洢��
	uint16_t cur_line;  //��ǰ��ʾ��
	uint16_t cur_page;  //��ǰ��ʾҳ��
    uint16_t mem_page;  //��¼��ҳ��
}LcdData,*pLcdData;

extern LcdData mUserLcdData; //LCD����ʾ�ṹ�����

void Lcd_Pin_Init(void);
void Lcd_Init(void);
void Lcd_Write_Cmd(uint8_t cmd);
void Lcd_Write_Data8(uint8_t data);
void Lcd_Write_Data16(uint16_t data);
void Lcd_Clear(uint16_t color);
void Lcd_Draw_Point(uint16_t x, uint16_t y, uint16_t color);
void Lcd_ShowChar(uint16_t x, uint16_t y, uint8_t num, Show_Size_Enum size, uint16_t Color);
void Lcd_ShowZHChar(uint16_t x, uint16_t y, uint8_t num, uint16_t Color);
void Lcd_ShowString(uint16_t x, uint16_t y, char *p, Show_Size_Enum size, uint16_t Color);
void Lcd_printf(char *fmt, ...);
void Lcd_Clear_Print(uint16_t color);
void Lcd_Clear_Osci(uint16_t color);
void Lcd_Osci_printf(char *fmt, ...);

#endif


