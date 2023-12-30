#ifndef __STEPMOTOR_H__
#define __STEPMOTOR_H__

#include "main.h"
#include ".\MALLOC\malloc.h"

/******************************************************************************************/
/* �������������غ� */
#define PULSE_REV 6400.0       /* ÿȦ��������ϸ����32�� */
#define MAX_STEP_ANGLE 1.8 / 32 /* ��С����(1.8/PULSE_REV) */

typedef struct
{
  int angle;                     /* ������Ҫ��ת�ĽǶ� */
  uint8_t dir;                   /* ���� */
  uint8_t en;                    /* ʹ�� */
  volatile uint32_t pulse_count; /* ���������¼ */
  volatile int add_pulse_count;  /* ��������ۼ� */
} STEPPER_MOTOR;

extern STEPPER_MOTOR g_stepper;

/******************************************************************************************/
#define T1_FREQ (84000000 / 84) /* Ƶ��ftֵ */
#define FSPR (200)              /* ���������Ȧ������*/
#define MICRO_STEP 4
#define SPR (FSPR * MICRO_STEP) /* ��Ȧ����Ҫ�������� */

#define ROUNDPS_2_STEPPS(rpm) ((rpm) * SPR / 60)              /* ���ݵ��ת�٣�r/min�������������٣�step/s�� */
#define MIDDLEVELOCITY(vo, vt) (((vo) + (vt)) / 2)            /* S�ͼӼ��ټ��ٶε��е��ٶ�  */
#define INCACCEL(vo, v, t) ((2 * ((v) - (vo))) / pow((t), 2)) /* �Ӽ��ٶ�:���ٶ�������   V - V0 = 1/2 * J * t^2 */
#define INCACCELSTEP(j, t) (((j) * pow((t), 3)) / 6.0f)       /* �Ӽ��ٶε�λ����(����)  S = 1/6 * J * t^3 */
#define ACCEL_TIME(t) ((t) / 2)                               /* �Ӽ��ٶκͼ����ٶε�ʱ������ȵ� */
#define SPEED_MIN (T1_FREQ / (65535.0f))                      /* ���Ƶ��/�ٶ� */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct
{
  int32_t vo;         /*  ���ٶ� ��λ step/s */
  int32_t vt;         /*  ĩ�ٶ� ��λ step/s */
  int32_t accel_step; /*  ���ٶεĲ�����λ step */
  int32_t decel_step; /*  ���ٶεĲ�����λ step */
  float *accel_tab;   /*  �ٶȱ�� ��λ step/s �������������Ƶ�� */
  float *decel_tab;   /*  �ٶȱ�� ��λ step/s �������������Ƶ�� */
  float *ptr;         /*  �ٶ�ָ�� */
  int32_t dec_point;  /*  ���ٵ� */
  int32_t step;
  int32_t step_pos;
} speed_calc_t;

typedef enum
{
  STATE_ACCEL = 1,    /* �������״̬ */
  STATE_AVESPEED = 2, /* �������״̬ */
  STATE_DECEL = 3,    /* �������״̬ */
  STATE_STOP = 0,     /* ���ֹͣ״̬ */
  STATE_IDLE = 4,     /* �������״̬ */
} motor_state_typedef;

#if 0
enum DIR
{
  CCW = 0, /*��ʱ��*/
  CW       /*˳ʱ��*/
};

enum EN
{
  EN_OFF = 0, /* ʧ���ѻ����� */
  EN_ON       /* ʹ���ѻ����� ʹ�ܺ���ֹͣ��ת */
};
#else
enum DIR
{
  CW = 0, /*˳ʱ��*/
  CCW     /*��ʱ��*/
};

enum EN
{
  EN_ON = 0, /* ʹ���ѻ����� ʹ�ܺ���ֹͣ��ת */
  EN_OFF,    /* ʧ���ѻ����� */
};
#endif

/******************************************************************************************/
/* ��������ӿ���� */
#define STEPPER_MOTOR_1 1
#define STEPPER_MOTOR_2 2
#define STEPPER_MOTOR_3 3

/* ����������Ŷ���*/
/*----------------------- �������ſ��� -----------------------------------*/
/* Ӳ���Ե�ƽ����ȡ�����������Ե� x = 1 ��Ч��x = 0ʱ��Ч*/
#define ST1_DIR(x) HAL_GPIO_WritePin(STEPPER1_DIR_GPIO_Port, STEPPER1_DIR_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define ST2_DIR(x) HAL_GPIO_WritePin(STEPPER2_DIR_GPIO_Port, STEPPER2_DIR_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET);

/*----------------------- �ѻ����ſ��� -----------------------------------*/
/* Ӳ���Ե�ƽ����ȡ�����������Ե� x = 1 ��Ч��x = 0ʱ��Ч*/
#define ST1_EN(x) HAL_GPIO_WritePin(STEPPER1_EN_GPIO_Port, STEPPER1_EN_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define ST2_EN(x) HAL_GPIO_WritePin(STEPPER2_EN_GPIO_Port, STEPPER2_EN_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)

/******************************************************************************************/
/* �ⲿ�ӿں���*/
void stepper_init(void);              /* ��������ӿڳ�ʼ�� */
void stepper_star(uint8_t motor_num); /* ����������� */
void stepper_stop(uint8_t motor_num); /* �رղ������ */

uint8_t calc_speed(int32_t vo, int32_t vt, float time);                                     /* �����ٶȱ� */
void stepmotor1_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step); /* ���1 S�ͼӼ����˶����ƺ��� */
void stepmotor2_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step); /* ���2 S�ͼӼ����˶����ƺ��� */
void stepper_set_angle(uint16_t angle, uint8_t dir, uint8_t motor_num);
#endif
