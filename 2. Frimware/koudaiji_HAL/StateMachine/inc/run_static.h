#ifndef __RUN_STATIC_H__
#define __RUN_STATIC_H__

#include "vision.h"

#define STATIC_ALLOW 10

typedef struct Run_Static_Str
{
    uint8_t step;  //运行的步数标记
    Res_Coord_Struct res_coord[10];  //AI模组识别到的物体坐标
    uint8_t res_num; //AI模组识别到的物体的数量
    uint16_t res_sum; //当前模式下识别物体总数

    uint16_t user_num; //时序控制计数用
    uint16_t code_corrd_x; //暂存当前物体x坐标
    bool same_flag;  //是否是同一幅图片标记
    uint8_t point_num; //指示个数控制
} Run_Static_Struct;

extern Run_Static_Struct Run_Static;

void Run_Mode_Static_Init(void);
void Run_Mode_Static_Enter(void);
void Run_Mode_Static_Exit(void);
void Run_Mode_Static_Run(void);

#endif



