#ifndef __INFARED_H__
#define __INFARED_H__

#include "gpio.h"
#include "stdbool.h"

union Infared_Data_Union  //�������ݹ�����
{
    uint8_t buf[4];
    uint32_t send_data;
};

typedef struct Infared_Str //����ң�ؽṹ��
{
    union Infared_Data_Union Infared_Data; //���͵�����
    volatile bool Infared_Is_Busy; //����æ��־
    volatile bool Infared_Is_Pause;
}Infared_Struct;


extern GPIO_Struct Infared_Pin; 
extern Infared_Struct Infared;

void Infared_Init(void);
void Infared_Send_Data(uint8_t user1, uint8_t user2, uint8_t data);
void Infared_Soft_Send_Data(uint8_t user1, uint8_t user2, uint8_t data);
void Infared_Send_OK(void);

#endif



