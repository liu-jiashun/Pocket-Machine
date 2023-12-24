#ifndef __RUN_SLOW_H__
#define __RUN_SLOW_H__

#include "vision.h"

#define SLOW_RIGHT_TO_LEFT 1  //通过宏定义选择视频的流动方向，目前支持从右向左和从上向下两种方向的检测
//#define SLOW_LEFT_TO_RIGHT 1
//#define SLOW_UP_TO_DOWN 1
//#define SLOW_DOWN_TO_UP 1

typedef struct Run_Slow_Str  //慢速模式运行相关的结构体
{
    uint8_t step;  //运行的步数标记
    Res_Coord_Struct res_coord[10];  //AI模组识别到的物体坐标
    uint8_t res_num;  //AI模组识别到的物体的数量
    bool first_flag;  //是否是首次检测标志
    Res_Coord_Struct last_res;  //记录当前页面最后一个物体的坐标
    uint16_t res_sum; //当前模式下识别物体总数
    uint8_t  need_point_num; //当前需要指示的物体的数量

    uint16_t user_num1;
    uint16_t user_num2;  //模式运行时的时间控制变量
    int16_t i; //模式运行时需要循环时的次数控制变量
    bool new_flag; //是否是新出现的物体的标志位
} Run_Slow_Struct;

extern Run_Slow_Struct Run_Slow;

void Run_Mode_Slow_Init(void);
void Run_Mode_Slow_Enter(void);
void Run_Mode_Slow_Exit(void);
void Run_Mode_Slow_Run(void);

#endif



