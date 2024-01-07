#ifndef __TFTLCD_H__
#define __TFTLCD_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include ".\MALLOC\malloc.h"
#include ".\tftlcd\st7789.h"

#define TFTLCD_MAX_LINES 10 // 最大行
#define TFTLCD_MAX_SIZE 32

void tftlcd_init(void); // 口袋机显示屏初始化

int tftlcd_printf(const char *fmt, ...); // tftlcd 打印函数

#endif
