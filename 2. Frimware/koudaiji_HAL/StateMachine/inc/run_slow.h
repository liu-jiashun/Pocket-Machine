#ifndef __RUN_SLOW_H__
#define __RUN_SLOW_H__

#include "vision.h"

#define SLOW_RIGHT_TO_LEFT 1  //ͨ���궨��ѡ����Ƶ����������Ŀǰ֧�ִ�������ʹ����������ַ���ļ��
//#define SLOW_LEFT_TO_RIGHT 1
//#define SLOW_UP_TO_DOWN 1
//#define SLOW_DOWN_TO_UP 1

typedef struct Run_Slow_Str  //����ģʽ������صĽṹ��
{
    uint8_t step;  //���еĲ������
    Res_Coord_Struct res_coord[10];  //AIģ��ʶ�𵽵���������
    uint8_t res_num;  //AIģ��ʶ�𵽵����������
    bool first_flag;  //�Ƿ����״μ���־
    Res_Coord_Struct last_res;  //��¼��ǰҳ�����һ�����������
    uint16_t res_sum; //��ǰģʽ��ʶ����������
    uint8_t  need_point_num; //��ǰ��Ҫָʾ�����������

    uint16_t user_num1;
    uint16_t user_num2;  //ģʽ����ʱ��ʱ����Ʊ���
    int16_t i; //ģʽ����ʱ��Ҫѭ��ʱ�Ĵ������Ʊ���
    bool new_flag; //�Ƿ����³��ֵ�����ı�־λ
} Run_Slow_Struct;

extern Run_Slow_Struct Run_Slow;

void Run_Mode_Slow_Init(void);
void Run_Mode_Slow_Enter(void);
void Run_Mode_Slow_Exit(void);
void Run_Mode_Slow_Run(void);

#endif



