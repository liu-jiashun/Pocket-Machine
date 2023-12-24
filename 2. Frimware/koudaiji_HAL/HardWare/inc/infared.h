#ifndef __INFARED_H__
#define __INFARED_H__

#include "gpio.h"
#include "stdbool.h"

union Infared_Data_Union  //红外数据共用体
{
    uint8_t buf[4];
    uint32_t send_data;
};

typedef struct Infared_Str //红外遥控结构体
{
    union Infared_Data_Union Infared_Data; //发送的数据
    volatile bool Infared_Is_Busy; //红外忙标志
    volatile bool Infared_Is_Pause;
}Infared_Struct;


extern GPIO_Struct Infared_Pin; 
extern Infared_Struct Infared;

void Infared_Init(void);
void Infared_Send_Data(uint8_t user1, uint8_t user2, uint8_t data);
void Infared_Soft_Send_Data(uint8_t user1, uint8_t user2, uint8_t data);
void Infared_Send_OK(void);

#endif



