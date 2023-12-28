#ifndef __STEPMOTOR_H__
#define __STEPMOTOR_H__

#include "main.h"
#include ".\MALLOC\malloc.h"

/******************************************************************************************/
#define T1_FREQ (84000000 / 84) /* 频率ft值 */
#define FSPR 200                      /* 步进电机单圈步数 */
#define MICRO_STEP 8
#define SPR (FSPR * MICRO_STEP) /* 单圈所需要的脉冲数 */

#define ROUNDPS_2_STEPPS(rpm) ((rpm) * SPR / 60)              /* 根据电机转速（r/min），计算电机步速（step/s） */
#define MIDDLEVELOCITY(vo, vt) (((vo) + (vt)) / 2)            /* S型加减速加速段的中点速度  */
#define INCACCEL(vo, v, t) ((2 * ((v) - (vo))) / pow((t), 2)) /* 加加速度:加速度增加量   V - V0 = 1/2 * J * t^2 */
#define INCACCELSTEP(j, t) (((j) * pow((t), 3)) / 6.0f)       /* 加加速段的位移量(步数)  S = 1/6 * J * t^3 */
#define ACCEL_TIME(t) ((t) / 2)                               /* 加加速段和减加速段的时间是相等的 */
#define SPEED_MIN (T1_FREQ / (65535.0f))                      /* 最低频率/速度 */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct
{
  int32_t vo;         /*  初速度 单位 step/s */
  int32_t vt;         /*  末速度 单位 step/s */
  int32_t accel_step; /*  加速段的步数单位 step */
  int32_t decel_step; /*  加速段的步数单位 step */
  float *accel_tab;   /*  速度表格 单位 step/s 步进电机的脉冲频率 */
  float *decel_tab;   /*  速度表格 单位 step/s 步进电机的脉冲频率 */
  float *ptr;         /*  速度指针 */
  int32_t dec_point;  /*  减速点 */
  int32_t step;
  int32_t step_pos;
} speed_calc_t;

typedef enum
{
  STATE_ACCEL = 1,    /* 电机加速状态 */
  STATE_AVESPEED = 2, /* 电机匀速状态 */
  STATE_DECEL = 3,    /* 电机减速状态 */
  STATE_STOP = 0,     /* 电机停止状态 */
  STATE_IDLE = 4,     /* 电机空闲状态 */
} motor_state_typedef;

enum DIR
{
  CCW = 0, /*逆时针*/
  CW       /*顺时针*/
};

enum EN
{
  EN_OFF = 0, /* 失能脱机引脚 */
  EN_ON       /* 使能脱机引脚 使能后电机停止旋转 */
};

/******************************************************************************************/
/* 步进电机接口序号 */
#define STEPPER_MOTOR_1 1
#define STEPPER_MOTOR_2 2
#define STEPPER_MOTOR_3 3

/* 步进电机引脚定义*/
/*----------------------- 方向引脚控制 -----------------------------------*/
/* 硬件对电平做了取反操作，所以当 x = 1 有效，x = 0时无效*/
#define ST1_DIR(x) HAL_GPIO_WritePin(STEPPER1_DIR_GPIO_Port, STEPPER1_DIR_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define ST2_DIR(x) HAL_GPIO_WritePin(STEPPER2_DIR_GPIO_Port, STEPPER2_DIR_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET);

/*----------------------- 脱机引脚控制 -----------------------------------*/
/* 硬件对电平做了取反操作，所以当 x = 1 有效，x = 0时无效*/
#define ST1_EN(x) HAL_GPIO_WritePin(STEPPER1_EN_GPIO_Port, STEPPER1_EN_Pin, x == 0? GPIO_PIN_SET : GPIO_PIN_RESET)
#define ST2_EN(x) HAL_GPIO_WritePin(STEPPER2_EN_GPIO_Port, STEPPER2_EN_Pin, x == 0? GPIO_PIN_SET : GPIO_PIN_RESET)

/******************************************************************************************/
/* 外部接口函数*/
void stepper_init(void);              /* 步进电机接口初始化 */
void stepper_star(uint8_t motor_num); /* 开启步进电机 */
void stepper_stop(uint8_t motor_num); /* 关闭步进电机 */

uint8_t calc_speed(int32_t vo, int32_t vt, float time);                                     /* 计算速度表 */
void stepmotor1_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step); /* 电机1 S型加减速运动控制函数 */
void stepmotor2_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step); /* 电机2 S型加减速运动控制函数 */

#endif
