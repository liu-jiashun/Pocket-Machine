#include "app.h"
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
void System_Start(void)
{
	uint8_t sys_start_task_id = 0;

	// �豸������ʼ��

	led_init();
	beep_init();
	touchkey_init();
	matrixkey_init();
	ST7789_Init();
	diwenlcd_init();

	my_mem_init(SRAMIN); /* ��ʼ���ڲ�SRAM�ڴ�� */

	// ϵͳ����
	sys_start_task_id = stim_runlater(100, system_start_task); // Ӳ����ʼ����� 100ms����������
	stim_remove_event(sys_start_task_id);											 // ��ϵͳ����������ɺ�ȡ��������
}
