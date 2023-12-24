#ifndef __BEEP_H__
#define __BEEP_H__

#include "gpio.h"

typedef enum
{
    BEEP_OFF = 0,
    BEEP_ON,
} BEEP_State;  //·äÃùÆ÷×´Ì¬Ã¶¾Ù

extern GPIO_Struct BEEP_Pin;

void Beep_Init(void);
void Beep_Control(BEEP_State state);

#endif



