#ifndef __CORE_H__
#define __CORE_H__

#include "stm32f4xx_hal.h"

void SystemClock_Config(void);
void Error_Handler(void);
void Soft_Delay_Us(uint16_t time);

#endif


