#ifndef __TRIPOD_H__
#define __TRIPOD_H__

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define X_STEP_ANGLE 59.5f  //��̨X������һ����Ҫ�Ĳ���
#define Y_STEP_ANGLE 60.0f  //��̨Y������һ����Ҫ�Ĳ���
#define DIS_X   976			// ��Ļ�ֱ�����1366  768  �����70cm*39.2cm ������̨����Ļ������50����ô�˴���ֵ��50/70*1366
#define DIS_Y 768

extern uint16_t Y_OFFSET;

typedef struct Tripod_Str  //��̨��ؽṹ��
{
    volatile uint32_t step_num;  //��Ҫ���еĲ���
    volatile uint32_t step_code;
    uint16_t coord;  //��ǰ����
    float angle;  //��ǰ�Ƕ�
    volatile bool is_limit;  //�Ƿ񵽴���λ
    volatile bool is_running;  //�Ƿ��������� ��volatile�������һ�������Ż��ı�����
}Tripod_Struct; 

extern GPIO_Struct Tripod_X_DIR, Tripod_X_EN, Tripod_X_STEP, Tripod_X_LIMIT, Tripod_Y_DIR, Tripod_Y_EN, Tripod_Y_STEP, Tripod_Y_LIMIT;
extern Tripod_Struct Tripod_X, Tripod_Y;

void Tripod_Init(void);
void Tripod_X_Dir(uint8_t dir);
void Tripod_X_Step(uint16_t num);

void Tripod_Y_Dir(uint8_t dir);
void Tripod_Y_Step(uint16_t num);
void Tripod_Key_Scan(void);
void Tripod_X_Go_Mechzero(void);
void Tripod_Y_Go_Mechzero(void);
void Tripod_X_Angle(float x_dat);
void Tripod_Y_Angle(float y_dat);
void Tripod_X_Coord(int x_dat);
void Tripod_Y_Coord(int y_dat);

#endif



