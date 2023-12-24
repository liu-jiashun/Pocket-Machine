#ifndef __RUN_MANUAL_H__
#define __RUN_MANUAL_H__

#include <stdint.h>

typedef struct Run_Manual_Str  //手动模式运行结构体
{
    uint8_t step;  //运行步数标记
    uint8_t mode;  //0:输入坐标，1:输入距离（cm）
    uint16_t target_x;
    uint16_t target_y;  //手动输入的坐标值
    uint16_t res_sum; //当前模式下识别物体总数
} Run_Manual_Struct;

extern Run_Manual_Struct Run_Manual;

extern Run_Manual_Struct Run_Manual;

void Run_Mode_Manual_Init(void);
void Run_Mode_Manual_Enter(void);
void Run_Mode_Manual_Exit(void);
void Run_Mode_Manual_Run(void);

#endif



