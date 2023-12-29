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
 * @brief     :数据接收处理任务
 * @attention :
 */
void data_receive_task(void)
{
	dgus_recv_data(); // 接收迪文屏数据
}

/**
 * @brief     :调试单元任务
 * @attention :
 */
void debug_task(void)
{
	// shellTask(&shell);
}

/**
 * @brief     :系统启动任务
 * @attention :
 */
void system_start_task(void)
{
	stim_loop(200, sys_task, STIM_LOOP_FOREVER);				 // 系统运行指示灯任务（200ms）
	stim_loop(20, button_task, STIM_LOOP_FOREVER);			 // 按键事件处理任务（20ms）
	stim_loop(50, data_receive_task, STIM_LOOP_FOREVER); // 数据接收任务（50ms）

#ifdef __Debug
	stim_loop(5, debug_task, STIM_LOOP_FOREVER); // 调试单元任务
#endif

	// 开始
	// voice_say(DEVICE_STARTED); // 设备启动播报
}

/**
 * @brief     :系统硬件初始化
 * @attention :
 */
void stim_start_task(void)
{
	my_mem_init(SRAMIN); /* 内部SRAM内存池初始化*/
	_Debug_Shell_Init(); /* 调试接口初始化 */

	// 设备驱动初始化
	led_init();				/* 口袋机LED初始化 */
	beep_init();			/* 口袋机蜂鸣器初始化 */
	touchkey_init();	/* 口袋机触摸按键初始化 */
	ST7789_Init();		/* 口袋机TFTLCD初始化 */
	matrixkey_init(); /* 5 x 4 矩阵键盘初始化 */
	diwenlcd_init();	/* 迪文屏初始化 */
	voice_init();			/* 语音初始化 */
	tirpod_init();		/* 云台初始化 */
	vision_init();		/* 视觉识别初始化 */

	// 任务调度系统启动
	system_start_task(); // 系统启动任务全部任务
}
