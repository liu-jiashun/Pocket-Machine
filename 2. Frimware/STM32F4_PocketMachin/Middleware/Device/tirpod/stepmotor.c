#include "stepmotor.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>

/**
 * @brief     :步进电机初始化
 * @attention :
 */
void stepper_init(void)
{
  // 引脚初始化，gpio.c 文件中已完成

  /* 启动电机定时器 */
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim4);

  /* 失能电机 */
  ST1_EN(EN_OFF);
  ST2_EN(EN_OFF);
}

/**
 * @brief     :开启步进电机
 * @param     motor_num :步进电机接口序号
 * @attention :
 */
void stepper_star(uint8_t motor_num)
{
  /* 开启对应PWM通道 */
  switch (motor_num)
  {
  case STEPPER_MOTOR_1:
    HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);
    break;
  case STEPPER_MOTOR_2:
    HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
    break;
  }
}

/**
 * @brief     :停止步进电机
 * @param     motor_num :步进电机接口序号
 * @attention :
 */
void stepper_stop(uint8_t motor_num)
{
  /* 关闭对应PWM通道 */
  switch (motor_num)
  {
  case STEPPER_MOTOR_1:
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
    break;
  case STEPPER_MOTOR_2:
    HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1);
    break;
  default:
    break;
  }
}

/****************************************S型加减速运动*****************************************************/
volatile int32_t g_step_pos = 0;              /* 当前位置 */
volatile uint16_t g_toggle_pulse = 0;         /* 脉冲频率控制 */
motor_state_typedef g_motor_sta = STATE_IDLE; /* 电机状态 */
speed_calc_t g_calc_t = {0};

__IO uint32_t g_add_pulse_count = 0; /* 脉冲个数累计 */

/**
 * @brief       速度表计算函数
 * @param       vo,初速度;vt,末速度;time,加速时间
 * @retval      TRUE：成功；FALSE：失败
 */
uint8_t calc_speed(int32_t vo, int32_t vt, float time)
{
  uint8_t is_dec = FALSE;
  int32_t i = 0;
  int32_t vm = 0;             /* 中间点速度 */
  int32_t inc_acc_stp = 0;    /* 加加速所需的步数 */
  int32_t dec_acc_stp = 0;    /* 减加速所需的步数 */
  int32_t accel_step = 0;     /* 加速或减速需要的步数 */
  float jerk = 0;             /* 加加速度 */
  float ti = 0;               /* 时间间隔 dt */
  float sum_t = 0;            /* 时间累加量 */
  float delta_v = 0;          /* 速度的增量dv */
  float ti_cube = 0;          /* 时间间隔的立方 */
  float *velocity_tab = NULL; /* 速度表格指针 */

  if (vo > vt)                          /* 初速度比末速度大,做减速运动,数值变化跟加速运动相同 */
  {                                     /* 只是建表的时候注意将速度倒序 */
    is_dec = TRUE;                      /* 减速段 */
    g_calc_t.vo = ROUNDPS_2_STEPPS(vt); /* 转换单位 起速:step/s */
    g_calc_t.vt = ROUNDPS_2_STEPPS(vo); /* 转换单位 末速:step/s */
  }
  else
  {
    is_dec = FALSE; /* 加速段 */
    g_calc_t.vo = ROUNDPS_2_STEPPS(vo);
    g_calc_t.vt = ROUNDPS_2_STEPPS(vt);
  }

  time = ACCEL_TIME(time); /* 得到加加速段的时间 */
  // printf("time=%f\r\n", time);
  vm = (g_calc_t.vo + g_calc_t.vt) / 2; /* 计算中点速度 */

  jerk = fabs(2.0f * (vm - g_calc_t.vo) / (time * time)); /* 根据中点速度计算加加速度 */

  inc_acc_stp = (int32_t)(g_calc_t.vo * time + INCACCELSTEP(jerk, time)); /* 加加速需要的步数 */

  dec_acc_stp = (int32_t)((g_calc_t.vt + g_calc_t.vo) * time - inc_acc_stp); /* 减加速需要的步数 S = vt * time - S1 */

  /* 申请内存空间存放速度表 */
  accel_step = dec_acc_stp + inc_acc_stp; /* 加速需要的步数 */
  if (accel_step % 2 != 0)                /* 由于浮点型数据转换成整形数据带来了误差,所以这里加1 */
    accel_step += 1;
  /* mallo申请内存空间,记得释放 */
  velocity_tab = (float *)(mymalloc(SRAMIN, ((accel_step + 1) * sizeof(float))));
  if (velocity_tab == NULL)
  {
    // printf("内存不足!请修改参数\r\n");
    return FALSE;
  }
  /*
   * 目标的S型速度曲线是对时间的方程,但是在控制电机的时候则是以步进的方式控制,所以这里对V-t曲线做转换
   * 得到V-S曲线,计算得到的速度表是关于步数的速度值.使得步进电机每一步都在控制当中
   */
  /* 计算第一步速度,根据第一步的速度值达到下一步的时间 */
  ti_cube = 6.0f * 1.0f / jerk;  /* 根据位移和时间的公式S = 1/6 * J * ti^3 第1步的时间:ti^3 = 6 * 1 / jerk */
  ti = pow(ti_cube, (1 / 3.0f)); /* ti */
  sum_t = ti;
  delta_v = 0.5f * jerk * pow(sum_t, 2); /* 第一步的速度 */
  velocity_tab[0] = g_calc_t.vo + delta_v;

  /*****************************************************/
  if (velocity_tab[0] <= SPEED_MIN) /* 以当前定时器频率所能达到的最低速度 */
    velocity_tab[0] = SPEED_MIN;

  /*****************************************************/

  for (i = 1; i < accel_step; i++)
  {
    /* 步进电机的速度就是定时器脉冲输出频率,可以计算出每一步的时间 */
    /* 得到第i-1步的时间 */
    ti = 1.0f / velocity_tab[i - 1]; /* 电机每走一步的时间 ti = 1 / Vn-1 */
    /* 加加速段速度计算 */
    if (i < inc_acc_stp)
    {
      sum_t += ti;                             /* 从0开始到i的时间累积 */
      delta_v = 0.5f * jerk * pow(sum_t, 2);   /* 速度的变化量: dV = 1/2 * jerk * ti^2 */
      velocity_tab[i] = g_calc_t.vo + delta_v; /* 得到加加速段每一步对应的速度 */
      /* 当最后一步的时候,时间并不严格等于time,所以这里要稍作处理,作为减加速段的时间 */
      if (i == inc_acc_stp - 1)
        sum_t = fabs(sum_t - time);
    }
    /* 减加速段速度计算 */
    else
    {
      sum_t += ti;                                        /* 时间累计 */
      delta_v = 0.5f * jerk * pow(fabs(time - sum_t), 2); /* dV = 1/2 * jerk *(T-t)^2 看这个逆向看减加速的图 */
      velocity_tab[i] = g_calc_t.vt - delta_v;            /* V = vt - delta_v */
      if (velocity_tab[i] >= g_calc_t.vt)
      {
        accel_step = i;
        break;
      }
    }
  }
  if (is_dec == TRUE) /* 减速 */
  {
    float tmp_Speed = 0;
    /* 倒序排序 */
    for (i = 0; i < (accel_step / 2); i++)
    {
      tmp_Speed = velocity_tab[i];
      velocity_tab[i] = velocity_tab[accel_step - 1 - i]; /* 头尾速度对换 */
      velocity_tab[accel_step - 1 - i] = tmp_Speed;
    }

    g_calc_t.decel_tab = velocity_tab; /* 减速段速度表 */
    g_calc_t.decel_step = accel_step;  /* 减速段的总步数 */
  }
  else /* 加速 */
  {
    g_calc_t.accel_tab = velocity_tab; /* 加速段速度表 */
    g_calc_t.accel_step = accel_step;  /* 加速段的总步数 */
  }
  return TRUE;
}

/**
 * @brief       电机1 S型加减速运动
 * @param       vo:初速度;vt:末速度;AcTime:加速时间;DeTime:减速时间;step:步数;
 * @retval      无
 */
void stepmotor1_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step)
{
  if (calc_speed(vo, vt, AcTime) == FALSE) /* 计算出加速段的速度和步数 */
    return;
  if (calc_speed(vt, vo, DeTime) == FALSE) /* 计算出减速段的速度和步数 */
    return;

  if (step < 0)
  {
    step = -step;
    ST1_DIR(CCW);
  }
  else
  {
    ST1_DIR(CW);
  }

  if (step >= (g_calc_t.decel_step + g_calc_t.accel_step)) /* 当总步数大于等于加减速度步数相加时，才可以实现完整的S形加减速 */
  {
    g_calc_t.step = step;
    g_calc_t.dec_point = g_calc_t.step - g_calc_t.decel_step; /* 开始减速的步数 */
  }
  else /* 步数不足以进行足够的加减速 */
  {
    /* 步数不足不足以运动，要把前面申请的速度表所占内存释放，以便后续可重复申请 */
    myfree(SRAMIN, g_calc_t.accel_tab); /* 释放加速段速度表 */
    myfree(SRAMIN, g_calc_t.decel_tab); /* 释放减速段速度表 */
    // printf("步数不足，参数设置错误!\r\n");
    return;
  }
  g_calc_t.step_pos = 0;
  g_motor_sta = STATE_ACCEL; /* 电机为加速状态 */

  g_calc_t.ptr = g_calc_t.accel_tab; /* 把加速段的速度表存储到ptr里边 */
  g_toggle_pulse = (uint32_t)(T1_FREQ / (*g_calc_t.ptr));
  g_calc_t.ptr++;

  __HAL_TIM_SET_COUNTER(&htim4, 0);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, (uint16_t)(g_toggle_pulse / 2)); /*  设置定时器比较值 */
  HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);                                   /* 使能定时器通道 */
  ST1_EN(EN_ON);
}

/**
 * @brief       电机2 S型加减速运动
 * @param       vo:初速度;vt:末速度;AcTime:加速时间;DeTime:减速时间;step:步数;
 * @retval      无
 */
void stepmotor2_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step)
{
  if (calc_speed(vo, vt, AcTime) == FALSE) /* 计算出加速段的速度和步数 */
    return;
  if (calc_speed(vt, vo, DeTime) == FALSE) /* 计算出减速段的速度和步数 */
    return;

  if (step < 0)
  {
    step = -step;
    ST2_DIR(CCW);
  }
  else
  {
    ST2_DIR(CW);
  }

  if (step >= (g_calc_t.decel_step + g_calc_t.accel_step)) /* 当总步数大于等于加减速度步数相加时，才可以实现完整的S形加减速 */
  {
    g_calc_t.step = step;
    g_calc_t.dec_point = g_calc_t.step - g_calc_t.decel_step; /* 开始减速的步数 */
  }
  else /* 步数不足以进行足够的加减速 */
  {
    /* 步数不足不足以运动，要把前面申请的速度表所占内存释放，以便后续可重复申请 */
    myfree(SRAMIN, g_calc_t.accel_tab); /* 释放加速段速度表 */
    myfree(SRAMIN, g_calc_t.decel_tab); /* 释放减速段速度表 */
    // printf("步数不足，参数设置错误!\r\n");
    return;
  }
  g_calc_t.step_pos = 0;
  g_motor_sta = STATE_ACCEL; /* 电机为加速状态 */

  g_calc_t.ptr = g_calc_t.accel_tab; /* 把加速段的速度表存储到ptr里边 */
  g_toggle_pulse = (uint32_t)(T1_FREQ / (*g_calc_t.ptr));
  g_calc_t.ptr++;

  __HAL_TIM_SET_COUNTER(&htim1, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)(g_toggle_pulse / 2)); /*  设置定时器比较值 */
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);                                   /* 使能定时器通道 */
  ST2_EN(EN_ON);
}

/**
 * @brief  定时器比较中断
 * @param  htim：定时器句柄指针
 * @note   无
 * @retval 无
 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  volatile uint32_t Tim_Count = 0;
  volatile uint32_t tmp = 0;
  volatile float Tim_Pulse = 0;
  volatile static uint8_t i = 0;

  if (htim->Instance == TIM1)
  {
    i++;        /* 定时器中断次数计数值 */
    if (i == 2) /* 2次，说明已经输出一个完整脉冲 */
    {
      i = 0;        /* 清零定时器中断次数计数值 */
      g_step_pos++; /* 当前位置 */
      if ((g_motor_sta != STATE_IDLE) && (g_motor_sta != STATE_STOP))
      {
        g_calc_t.step_pos++;
      }
      switch (g_motor_sta)
      {
      case STATE_ACCEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr);        /* 由速度表得到每一步的定时器计数值 */
        g_calc_t.ptr++;                               /* 取速度表的下一位 */
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);   /* 翻转模式C需要除以2 */
        if (g_calc_t.step_pos >= g_calc_t.accel_step) /* 当大于加速段步数就进入匀速 */
        {
          myfree(SRAMIN, g_calc_t.accel_tab); /* 运动完要释放内存 */
          g_motor_sta = STATE_AVESPEED;
        }
        break;
      case STATE_DECEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr); /* 由速度表得到每一步的定时器计数值 */
        g_calc_t.ptr++;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.step)
        {
          myfree(SRAMIN, g_calc_t.decel_tab); /* 运动完要释放内存 */
          g_motor_sta = STATE_STOP;
        }
        break;
      case STATE_AVESPEED:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / g_calc_t.vt;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.dec_point)
        {
          g_calc_t.ptr = g_calc_t.decel_tab; /* 将减速段的速度表赋值给ptr */
          g_motor_sta = STATE_DECEL;
        }
        break;
      case STATE_STOP:
        HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1); /* 开启对应PWM通道 */
        g_motor_sta = STATE_IDLE;
        break;
      case STATE_IDLE:
        break;
      }
    }
    /*  设置比较值 */
    Tim_Count = __HAL_TIM_GET_COUNTER(&htim1);
    tmp = 0xFFFF & (Tim_Count + g_toggle_pulse);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, tmp);
  }
  else if (htim->Instance == TIM4)
  {
    i++;        /* 定时器中断次数计数值 */
    if (i == 2) /* 2次，说明已经输出一个完整脉冲 */
    {
      i = 0;        /* 清零定时器中断次数计数值 */
      g_step_pos++; /* 当前位置 */
      if ((g_motor_sta != STATE_IDLE) && (g_motor_sta != STATE_STOP))
      {
        g_calc_t.step_pos++;
      }
      switch (g_motor_sta)
      {
      case STATE_ACCEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr);        /* 由速度表得到每一步的定时器计数值 */
        g_calc_t.ptr++;                               /* 取速度表的下一位 */
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);   /* 翻转模式C需要除以2 */
        if (g_calc_t.step_pos >= g_calc_t.accel_step) /* 当大于加速段步数就进入匀速 */
        {
          myfree(SRAMIN, g_calc_t.accel_tab); /* 运动完要释放内存 */
          g_motor_sta = STATE_AVESPEED;
        }
        break;
      case STATE_DECEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr); /* 由速度表得到每一步的定时器计数值 */
        g_calc_t.ptr++;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.step)
        {
          myfree(SRAMIN, g_calc_t.decel_tab); /* 运动完要释放内存 */
          g_motor_sta = STATE_STOP;
        }
        break;
      case STATE_AVESPEED:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / g_calc_t.vt;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.dec_point)
        {
          g_calc_t.ptr = g_calc_t.decel_tab; /* 将减速段的速度表赋值给ptr */
          g_motor_sta = STATE_DECEL;
        }
        break;
      case STATE_STOP:
        HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1); /* 开启对应PWM通道 */
        g_motor_sta = STATE_IDLE;
        break;
      case STATE_IDLE:
        break;
      }
    }
    /*  设置比较值 */
    Tim_Count = __HAL_TIM_GET_COUNTER(&htim4);
    tmp = 0xFFFF & (Tim_Count + g_toggle_pulse);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tmp);
  }
}
