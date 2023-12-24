#ifndef __TOUCH_KEY_H__
#define __TOUCH_KEY_H__

#include "gpio.h"

typedef enum 
{
    TOUCH_KEY_NONE = 0,
    TOUCH_KEY_UP,
    TOUCH_KEY_DOWN,
    TOUCH_KEY_LEFT,
    TOUCH_KEY_RIGHT,
    TOUCH_KEY_A,
    TOUCH_KEY_B,
    TOUCH_KEY_C,
    TOUCH_KEY_D,
}TOUCH_KEY_Enum;

extern GPIO_Struct Touch_Key_Up, Touch_Key_Down, Touch_Key_Left, Touch_Key_Right, Touch_Key_A, Touch_Key_B, Touch_Key_C, Touch_Key_D;
extern TOUCH_KEY_Enum Touch_Key_Value;

void Touch_Key_Init(void);
void Touch_Key_Scan(void);

#endif


