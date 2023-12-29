#include "app.h"
#include "tim.h"
#include "_Debug.h"
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
#include ".\voice\voice.h"
#include ".\vision\vision.h"

#include <stdio.h>

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
 * @brief     :���ݽ��մ�������
 * @attention :
 */
void data_receive_task(void)
{
	dgus_recv_data(); // ���յ���������
}

/**
 * @brief     :���Ե�Ԫ����
 * @attention :
 */
void debug_task(void)
{
	// shellTask(&shell);
}

/**
 * @brief     :ϵͳ��������
 * @attention :
 */
void system_start_task(void)
{
	stim_loop(200, sys_task, STIM_LOOP_FOREVER);				 // ϵͳ����ָʾ������200ms��
	stim_loop(20, button_task, STIM_LOOP_FOREVER);			 // �����¼���������20ms��
	stim_loop(50, data_receive_task, STIM_LOOP_FOREVER); // ���ݽ�������50ms��

#ifdef __Debug
	stim_loop(5, debug_task, STIM_LOOP_FOREVER); // ���Ե�Ԫ����
#endif

	// ��ʼ
	// voice_say(DEVICE_STARTED); // �豸��������
}

/**
 * @brief     :ϵͳӲ����ʼ��
 * @attention :
 */
void stim_start_task(void)
{
	my_mem_init(SRAMIN); /* �ڲ�SRAM�ڴ�س�ʼ��*/
	_Debug_Shell_Init(); /* ���Խӿڳ�ʼ�� */

	// �豸������ʼ��
	led_init();				/* �ڴ���LED��ʼ�� */
	beep_init();			/* �ڴ�����������ʼ�� */
	touchkey_init();	/* �ڴ�������������ʼ�� */
	ST7789_Init();		/* �ڴ���TFTLCD��ʼ�� */
	matrixkey_init(); /* 5 x 4 ������̳�ʼ�� */
	diwenlcd_init();	/* ��������ʼ�� */
	voice_init();			/* ������ʼ�� */
	tirpod_init();		/* ��̨��ʼ�� */
	vision_init();		/* �Ӿ�ʶ���ʼ�� */

	// �������ϵͳ����
	system_start_task(); // ϵͳ��������ȫ������
}
