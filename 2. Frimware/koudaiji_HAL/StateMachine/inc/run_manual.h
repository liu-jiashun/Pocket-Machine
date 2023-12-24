#ifndef __RUN_MANUAL_H__
#define __RUN_MANUAL_H__

#include <stdint.h>

typedef struct Run_Manual_Str  //�ֶ�ģʽ���нṹ��
{
    uint8_t step;  //���в������
    uint8_t mode;  //0:�������꣬1:������루cm��
    uint16_t target_x;
    uint16_t target_y;  //�ֶ����������ֵ
    uint16_t res_sum; //��ǰģʽ��ʶ����������
} Run_Manual_Struct;

extern Run_Manual_Struct Run_Manual;

extern Run_Manual_Struct Run_Manual;

void Run_Mode_Manual_Init(void);
void Run_Mode_Manual_Enter(void);
void Run_Mode_Manual_Exit(void);
void Run_Mode_Manual_Run(void);

#endif



