#ifndef __DIWENLCD_H__
#define __DIWENLCD_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "main.h"
#include ".\libdgus_ii\dgus.h"

#define DIWENLCD_MAX_LINES 17 // 迪文屏打印终端最大行数
#define DIWENLCD_MAX_SIZES 48 // 迪文屏一行最大字节数

/**
 * @brief     :diwenlcd show work mode
 * @attention :
 */
typedef enum DIWENLCD_MODE
{
  DIWENLCD_MANUAL_MODE = 0,
  DIWENLCD_STATIC_MODE,
  DIWENLCD_SLOW_MODE,
  DIWENLCD_FAST_MODE,
} diwenlcd_mode;

/**
 * @brief  MAX485 Working Status
 */
enum
{
  MAX485_RECEIVE = 0,
  MAX485_SEND,
};

/**
 * @brief     :MAX485 work mode switching
 */
#define MAX485_DR(x) HAL_GPIO_WritePin(MAX485_MODE_GPIO_Port, MAX485_MODE_Pin, x == MAX485_SEND ? GPIO_PIN_SET : GPIO_PIN_RESET)

void max485_init(void);
HAL_StatusTypeDef max485_send(const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef max485_receive(uint8_t *pData, uint16_t Size);

/**
 * @brief     :迪文屏相关
 * @attention :
 */
void diwenlcd_init(void);
int diwenlcd_printf(const char *fmt, ...);
void diwenlcd_time_show(uint32_t times);
void diwenlcd_mode_count(diwenlcd_mode mode, uint16_t count);
void diwenlcd_mode_show(diwenlcd_mode mode);
void diwenlcd_objects_type(char *str);
void diwenlcd_coord_show(uint16_t x_coord, uint16_t y_coord);

#endif
