#ifndef __VISION_H__
#define __VISION_H__

#include "usart.h"

typedef struct res_coord_str //物品坐标信息结构体
{
    uint16_t x;
    uint16_t y;
}Res_Coord_Struct;

typedef struct vision_coord_str  //AI模组数据包中包含的检测到的物体信息的结构体
{
    Res_Coord_Struct Res_Coord[10];
    uint8_t Res_Num;
}Vision_Coord_Struct, *Vision_Coord_Struct_p;

extern Uart_Receive_Struct Vision_Receive;
extern Vision_Coord_Struct Vision_Coord;

void Vision_Send_Cmd(uint8_t color, uint8_t shape);
uint8_t Vision_Dat_Explan(void);
static void Res_Sort(Vision_Coord_Struct_p wait_sort);
static void Res_Sort2(Vision_Coord_Struct_p wait_sort);

#endif



