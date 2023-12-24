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
 * @brief     :系统运行指示灯任务
 * @attention :
 */
void sys_task(void)
{
	static uint8_t flag = 0;
	led_ctl(LED0, flag ? ON : OFF);
	flag = !flag;
}

/**
 * @brief     :按键事件处理任务
 * @attention :
 */
void button_task(void)
{
	flex_button_scan(); // 按键事件处理 20ms
}

/**
 * @brief     :调试单元任务
 * @attention :
 */
void debug_task(void)
{
	dgus_recv_data(); // 接收迪文屏数据
}

/**
 * @brief     :系统启动任务
 * @attention :
 */
void system_start_task(void)
{
	stim_loop(200, sys_task, STIM_LOOP_FOREVER);	 // 系统运行指示灯任务（200ms）
	stim_loop(20, button_task, STIM_LOOP_FOREVER); // 按键事件处理任务（20ms）

#ifdef Debug
	stim_loop(5, debug_task, STIM_LOOP_FOREVER); // 调试单元任务
#endif
}

/**
 * @brief     :系统硬件初始化
 * @attention :
 */
void System_Start(void)
{
	uint8_t sys_start_task_id = 0;

	// 设备驱动初始化

	led_init();
	beep_init();
	touchkey_init();
	matrixkey_init();
	ST7789_Init();
	diwenlcd_init();

	my_mem_init(SRAMIN); /* 初始化内部SRAM内存池 */

	// 系统启动
	sys_start_task_id = stim_runlater(100, system_start_task); // 硬件初始化完成 100ms后启动任务
	stim_remove_event(sys_start_task_id);											 // 当系统启动任务完成后，取消该任务
}
