#ifndef __SYS_FONT_H__
#define __SYS_FONT_H__
#include <stdint.h>

typedef enum //字体大小枚举
{
	SHOW_SIZE_16 = 16, //1608字体
	SHOW_SIZE_32 = 32, //3216字体
}Show_Size_Enum;

extern Show_Size_Enum Show_Size;

extern const unsigned char asc2_3216[95][64];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char ZH_1616[][16];

#endif
