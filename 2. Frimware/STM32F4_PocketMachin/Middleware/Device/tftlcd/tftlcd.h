#ifndef __TFTLCD_H__
#define __TFTLCD_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include ".\MALLOC\malloc.h"
#include ".\tftlcd\st7789.h"

void tftlcd_init(void); // 口袋机显示屏初始化

int tftlcd_printf(const char *fmt, ...); // tftlcd 打印函数

#endif
