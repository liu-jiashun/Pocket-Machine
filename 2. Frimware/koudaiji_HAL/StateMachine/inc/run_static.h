#ifndef __RUN_STATIC_H__
#define __RUN_STATIC_H__

#include "vision.h"

#define STATIC_ALLOW 10

typedef struct Run_Static_Str
{
    uint8_t step;  //���еĲ������
    Res_Coord_Struct res_coord[10];  //AIģ��ʶ�𵽵���������
    uint8_t res_num; //AIģ��ʶ�𵽵����������
    uint16_t res_sum; //��ǰģʽ��ʶ����������

    uint16_t user_num; //ʱ����Ƽ�����
    uint16_t code_corrd_x; //�ݴ浱ǰ����x����
    bool same_flag;  //�Ƿ���ͬһ��ͼƬ���
    uint8_t point_num; //ָʾ��������
} Run_Static_Struct;

extern Run_Static_Struct Run_Static;

void Run_Mode_Static_Init(void);
void Run_Mode_Static_Enter(void);
void Run_Mode_Static_Exit(void);
void Run_Mode_Static_Run(void);

#endif



