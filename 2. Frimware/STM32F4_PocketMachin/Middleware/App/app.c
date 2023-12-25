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
 * @brief     :系统运行指示灯任务
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
void stim_start_task(void)
{
	// 设备驱动初始化
	led_init();					 /* 口袋机LED初始化 */
	beep_init();				 /* 口袋机蜂鸣器初始化 */
	touchkey_init();		 /* 口袋机触摸按键初始化 */
	matrixkey_init();		 /* 5 x 4 矩阵键盘初始化 */
	ST7789_Init();			 /* TFTLCD初始化 */
	diwenlcd_init();		 /* 迪文屏初始化 */
	my_mem_init(SRAMIN); /* 内部SRAM内存池初始化*/

	// 任务调度系统启动
	stim_runlater(100, system_start_task); // 100 ms后启动任务全部任务
}
