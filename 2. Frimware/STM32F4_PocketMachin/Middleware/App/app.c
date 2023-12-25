#include "app.h"

#include "tim.h"

#include ".\SmartTimer\smarttimer.h"
#include ".\MALLOC\malloc.h"

#include ".\power\power.h"
#include ".\led\led.h"
#include ".\beep\beep.h"
#include ".\touchkey\touchkey.h"
#include ".\matrixkey\matrixkey.h"
#include ".\tftlcd\st7789.h"
#include ".\diwenlcd\diwenlcd.h"
#include ".\tirpod\tirpod.h"

#include <stdio.h>

#define Debug

/**
 * @brief     :ϵͳ����ָʾ������
 * @attention :
 */
void sys_task(void)
{
	static uint8_t flag = 0;
	led_ctl(LED0, flag ? ON : OFF);
	flag = !flag;

	HAL_GPIO_TogglePin(MINI_LED0_GPIO_Port, MINI_LED0_Pin);
}

/**
 * @brief     :�����¼���������
 * @attention :
 */
void button_task(void)
{
	flex_button_scan(); // �����¼����� 20ms
}

/**
 * @brief     :���Ե�Ԫ����
 * @attention :
 */
void debug_task(void)
{
	dgus_recv_data(); // ���յ���������
}

/**
 * @brief     :ϵͳ��������
 * @attention :
 */
void system_start_task(void)
{
	stim_loop(200, sys_task, STIM_LOOP_FOREVER);	 // ϵͳ����ָʾ������200ms��
	stim_loop(20, button_task, STIM_LOOP_FOREVER); // �����¼���������20ms��

#ifdef Debug
	stim_loop(5, debug_task, STIM_LOOP_FOREVER); // ���Ե�Ԫ����
#endif
}

/**
 * @brief     :ϵͳӲ����ʼ��
 * @attention :
 */
void stim_start_task(void)
{
	// �豸������ʼ��
	led_init();					 /* �ڴ���LED��ʼ�� */
	beep_init();				 /* �ڴ�����������ʼ�� */
	touchkey_init();		 /* �ڴ�������������ʼ�� */
	matrixkey_init();		 /* 5 x 4 ������̳�ʼ�� */
	ST7789_Init();			 /* TFTLCD��ʼ�� */
	diwenlcd_init();		 /* ��������ʼ�� */
	my_mem_init(SRAMIN); /* �ڲ�SRAM�ڴ�س�ʼ��*/

	// �������ϵͳ����
	stim_runlater(100, system_start_task); // 100 ms����������ȫ������
}
