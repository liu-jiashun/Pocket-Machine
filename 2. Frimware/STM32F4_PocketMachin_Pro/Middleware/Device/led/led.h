#ifndef __LED_H__
#define __LED_H__

#include "main.h"

enum led
{
  LED0 = 0,
  LED1,
  LED2,
  LED3,
  LED4,
  LED5,
  LED6,
  LED7
};

void led_init(void);

void led_ctl(uint8_t led, uint8_t state);

#endif
