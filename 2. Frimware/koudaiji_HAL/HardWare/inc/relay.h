#ifndef __RELAY_H__
#define __RELAY_H__

#include "gpio.h"

typedef enum
{
    RELAY_ON = 0,
    RELAY_OFF,
} Relay_State;

extern GPIO_Struct Relay_Pin; 

void Relay_Init(void);
void Relay_Control(Relay_State state);

#endif


