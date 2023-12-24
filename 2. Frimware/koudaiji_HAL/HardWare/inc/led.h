#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"

typedef enum //LED��״̬ö��
{
    LED_ON = 0, //LED��
    LED_OFF, //LED��
} LED_State; 

extern GPIO_Struct LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7;

void Led_Init(void);
void Led_Control(GPIO_Struct_P led, LED_State state);

#endif
