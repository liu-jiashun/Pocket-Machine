#ifndef __POWER_H__
#define __POWER_H__

#include "gpio.h"

typedef enum
{
    POWER_ON = 0,
    POWER_OFF,
} Power_State;

extern GPIO_Struct Power_Pin; 

void Power_Init(void);
void Power_Control(Power_State state);

#endif



