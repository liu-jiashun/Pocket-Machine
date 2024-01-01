#include "stepmotor.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>

STEPPER_MOTOR g_stepper1;
uint32_t g_count_val1 = 0; /* ����ֵ */
uint32_t g_ccr_val1 = 500; /* �Ƚ�ֵ��ֵ */
uint8_t g_run_flag1 = 0;   /* ��־λ */

STEPPER_MOTOR g_stepper2;
uint32_t g_count_val2 = 0; /* ����ֵ */
uint32_t g_ccr_val2 = 500; /* �Ƚ�ֵ��ֵ */
uint8_t g_run_flag2 = 0;   /* ��־λ */

/**
 * @brief     :���������ʼ��
 * @attention :
 */
void stepper_init(void)
{
  // ���ų�ʼ����gpio.c �ļ��������

  /* ���������ʱ�� */
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim4);

  /* ʧ�ܵ�� */
  ST1_EN(EN_OFF);
  ST2_EN(EN_OFF);
}

/**
 * @brief     :�����������
 * @param     motor_num :��������ӿ����
 * @attention :
 */
void stepper_star(uint8_t motor_num)
{
  /* ������ӦPWMͨ�� */
  switch (motor_num)
  {
  case STEPPER_MOTOR_1:
    ST1_EN(EN_ON);
    HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);
    break;
  case STEPPER_MOTOR_2:
    ST2_EN(EN_ON);
    HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
    break;
  }
}

/**
 * @brief       �����������
 * @param       motor_num: ��������ӿ����
 * @param       dir      : ���������ת����
 * @retval      ��
 */
void stepper_star_dir(uint8_t motor_num, uint8_t dir)
{
  switch (motor_num)
  {
  /* ������ӦPWMͨ�� */
  case STEPPER_MOTOR_1:
  {
    ST1_DIR(dir);
    ST1_EN(EN_ON);
    HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);
    break;
  }
  case STEPPER_MOTOR_2:
  {
    ST2_DIR(dir);
    ST2_EN(EN_ON);
    HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
    break;
  }
  default:
    break;
  }
}

/**
 * @brief     :ֹͣ�������
 * @param     motor_num :��������ӿ����
 * @attention :
 */
void stepper_stop(uint8_t motor_num)
{
  /* �رն�ӦPWMͨ�� */
  switch (motor_num)
  {
  case STEPPER_MOTOR_1:
    ST1_EN(EN_OFF);
    HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1);
    break;
  case STEPPER_MOTOR_2:
    ST2_EN(EN_OFF);
    HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1);
    break;
  default:
    break;
  }
}

/**
 * @brief       ����Ҫת���ĽǶ�ת����������
 * @param       angle    : ��Ҫת���ĽǶ�ֵ
 * @param       dir      : ��ת����
 * @param       motor_num: ��������ӿ����
 * @retval      ��
 */
void stepper_set_angle(uint16_t angle, uint8_t dir, uint8_t motor_num)
{

  switch (motor_num)
  {
  case STEPPER_MOTOR_1:
    g_stepper1.pulse_count = angle / MAX_STEP_ANGLE;
    if (g_stepper1.pulse_count == 0)
    {
      stepper_stop(motor_num);
    }
    else
      stepper_star_dir(motor_num, dir);
    break;
  case STEPPER_MOTOR_2:
    g_stepper2.pulse_count = angle / MAX_STEP_ANGLE;
    if (g_stepper2.pulse_count == 0)
    {
      stepper_stop(motor_num);
    }
    else
      stepper_star_dir(motor_num, dir);
    break;
  }
}

/****************************************S�ͼӼ����˶�*****************************************************/
#ifdef S_CURVE
volatile int32_t g_step_pos = 0;              /* ��ǰλ�� */
volatile uint16_t g_toggle_pulse = 0;         /* ����Ƶ�ʿ��� */
motor_state_typedef g_motor_sta = STATE_IDLE; /* ���״̬ */
speed_calc_t g_calc_t = {0};

__IO uint32_t g_add_pulse_count = 0; /* ��������ۼ� */

/**
 * @brief       �ٶȱ���㺯��
 * @param       vo,���ٶ�;vt,ĩ�ٶ�;time,����ʱ��
 * @retval      TRUE���ɹ���FALSE��ʧ��
 */
uint8_t calc_speed(int32_t vo, int32_t vt, float time)
{
  uint8_t is_dec = FALSE;
  int32_t i = 0;
  int32_t vm = 0;             /* �м���ٶ� */
  int32_t inc_acc_stp = 0;    /* �Ӽ�������Ĳ��� */
  int32_t dec_acc_stp = 0;    /* ����������Ĳ��� */
  int32_t accel_step = 0;     /* ���ٻ������Ҫ�Ĳ��� */
  float jerk = 0;             /* �Ӽ��ٶ� */
  float ti = 0;               /* ʱ���� dt */
  float sum_t = 0;            /* ʱ���ۼ��� */
  float delta_v = 0;          /* �ٶȵ�����dv */
  float ti_cube = 0;          /* ʱ���������� */
  float *velocity_tab = NULL; /* �ٶȱ��ָ�� */

  if (vo > vt)                          /* ���ٶȱ�ĩ�ٶȴ�,�������˶�,��ֵ�仯�������˶���ͬ */
  {                                     /* ֻ�ǽ����ʱ��ע�⽫�ٶȵ��� */
    is_dec = TRUE;                      /* ���ٶ� */
    g_calc_t.vo = ROUNDPS_2_STEPPS(vt); /* ת����λ ����:step/s */
    g_calc_t.vt = ROUNDPS_2_STEPPS(vo); /* ת����λ ĩ��:step/s */
  }
  else
  {
    is_dec = FALSE; /* ���ٶ� */
    g_calc_t.vo = ROUNDPS_2_STEPPS(vo);
    g_calc_t.vt = ROUNDPS_2_STEPPS(vt);
  }

  time = ACCEL_TIME(time); /* �õ��Ӽ��ٶε�ʱ�� */
  // printf("time=%f\r\n", time);
  vm = (g_calc_t.vo + g_calc_t.vt) / 2; /* �����е��ٶ� */

  jerk = fabs(2.0f * (vm - g_calc_t.vo) / (time * time)); /* �����е��ٶȼ���Ӽ��ٶ� */

  inc_acc_stp = (int32_t)(g_calc_t.vo * time + INCACCELSTEP(jerk, time)); /* �Ӽ�����Ҫ�Ĳ��� */

  dec_acc_stp = (int32_t)((g_calc_t.vt + g_calc_t.vo) * time - inc_acc_stp); /* ��������Ҫ�Ĳ��� S = vt * time - S1 */

  /* �����ڴ�ռ����ٶȱ� */
  accel_step = dec_acc_stp + inc_acc_stp; /* ������Ҫ�Ĳ��� */
  if (accel_step % 2 != 0)                /* ���ڸ���������ת�����������ݴ��������,���������1 */
    accel_step += 1;
  /* mallo�����ڴ�ռ�,�ǵ��ͷ� */
  velocity_tab = (float *)(mymalloc(SRAMIN, ((accel_step + 1) * sizeof(float))));
  if (velocity_tab == NULL)
  {
    // printf("�ڴ治��!���޸Ĳ���\r\n");
    return FALSE;
  }
  /*
   * Ŀ���S���ٶ������Ƕ�ʱ��ķ���,�����ڿ��Ƶ����ʱ�������Բ����ķ�ʽ����,���������V-t������ת��
   * �õ�V-S����,����õ����ٶȱ��ǹ��ڲ������ٶ�ֵ.ʹ�ò������ÿһ�����ڿ��Ƶ���
   */
  /* �����һ���ٶ�,���ݵ�һ�����ٶ�ֵ�ﵽ��һ����ʱ�� */
  ti_cube = 6.0f * 1.0f / jerk;  /* ����λ�ƺ�ʱ��Ĺ�ʽS = 1/6 * J * ti^3 ��1����ʱ��:ti^3 = 6 * 1 / jerk */
  ti = pow(ti_cube, (1 / 3.0f)); /* ti */
  sum_t = ti;
  delta_v = 0.5f * jerk * pow(sum_t, 2); /* ��һ�����ٶ� */
  velocity_tab[0] = g_calc_t.vo + delta_v;

  /*****************************************************/
  if (velocity_tab[0] <= SPEED_MIN) /* �Ե�ǰ��ʱ��Ƶ�����ܴﵽ������ٶ� */
    velocity_tab[0] = SPEED_MIN;

  /*****************************************************/

  for (i = 1; i < accel_step; i++)
  {
    /* ����������ٶȾ��Ƕ�ʱ���������Ƶ��,���Լ����ÿһ����ʱ�� */
    /* �õ���i-1����ʱ�� */
    ti = 1.0f / velocity_tab[i - 1]; /* ���ÿ��һ����ʱ�� ti = 1 / Vn-1 */
    /* �Ӽ��ٶ��ٶȼ��� */
    if (i < inc_acc_stp)
    {
      sum_t += ti;                             /* ��0��ʼ��i��ʱ���ۻ� */
      delta_v = 0.5f * jerk * pow(sum_t, 2);   /* �ٶȵı仯��: dV = 1/2 * jerk * ti^2 */
      velocity_tab[i] = g_calc_t.vo + delta_v; /* �õ��Ӽ��ٶ�ÿһ����Ӧ���ٶ� */
      /* �����һ����ʱ��,ʱ�䲢���ϸ����time,��������Ҫ��������,��Ϊ�����ٶε�ʱ�� */
      if (i == inc_acc_stp - 1)
        sum_t = fabs(sum_t - time);
    }
    /* �����ٶ��ٶȼ��� */
    else
    {
      sum_t += ti;                                        /* ʱ���ۼ� */
      delta_v = 0.5f * jerk * pow(fabs(time - sum_t), 2); /* dV = 1/2 * jerk *(T-t)^2 ��������򿴼����ٵ�ͼ */
      velocity_tab[i] = g_calc_t.vt - delta_v;            /* V = vt - delta_v */
      if (velocity_tab[i] >= g_calc_t.vt)
      {
        accel_step = i;
        break;
      }
    }
  }
  if (is_dec == TRUE) /* ���� */
  {
    float tmp_Speed = 0;
    /* �������� */
    for (i = 0; i < (accel_step / 2); i++)
    {
      tmp_Speed = velocity_tab[i];
      velocity_tab[i] = velocity_tab[accel_step - 1 - i]; /* ͷβ�ٶȶԻ� */
      velocity_tab[accel_step - 1 - i] = tmp_Speed;
    }

    g_calc_t.decel_tab = velocity_tab; /* ���ٶ��ٶȱ� */
    g_calc_t.decel_step = accel_step;  /* ���ٶε��ܲ��� */
  }
  else /* ���� */
  {
    g_calc_t.accel_tab = velocity_tab; /* ���ٶ��ٶȱ� */
    g_calc_t.accel_step = accel_step;  /* ���ٶε��ܲ��� */
  }
  return TRUE;
}

/**
 * @brief       ���1 S�ͼӼ����˶�
 * @param       vo:���ٶ�;vt:ĩ�ٶ�;AcTime:����ʱ��;DeTime:����ʱ��;step:����;
 * @retval      ��
 */
void stepmotor1_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step)
{
  if (calc_speed(vo, vt, AcTime) == FALSE) /* ��������ٶε��ٶȺͲ��� */
    return;
  if (calc_speed(vt, vo, DeTime) == FALSE) /* ��������ٶε��ٶȺͲ��� */
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

  if (step >= (g_calc_t.decel_step + g_calc_t.accel_step)) /* ���ܲ������ڵ��ڼӼ��ٶȲ������ʱ���ſ���ʵ��������S�μӼ��� */
  {
    g_calc_t.step = step;
    g_calc_t.dec_point = g_calc_t.step - g_calc_t.decel_step; /* ��ʼ���ٵĲ��� */
  }
  else /* ���������Խ����㹻�ļӼ��� */
  {
    /* �������㲻�����˶���Ҫ��ǰ��������ٶȱ���ռ�ڴ��ͷţ��Ա�������ظ����� */
    myfree(SRAMIN, g_calc_t.accel_tab); /* �ͷż��ٶ��ٶȱ� */
    myfree(SRAMIN, g_calc_t.decel_tab); /* �ͷż��ٶ��ٶȱ� */
    printf("�������㣬�������ô���!\r\n");
    return;
  }
  g_calc_t.step_pos = 0;
  g_motor_sta = STATE_ACCEL; /* ���Ϊ����״̬ */

  g_calc_t.ptr = g_calc_t.accel_tab; /* �Ѽ��ٶε��ٶȱ�洢��ptr��� */
  g_toggle_pulse = (uint32_t)(T1_FREQ / (*g_calc_t.ptr));
  g_calc_t.ptr++;

  __HAL_TIM_SET_COUNTER(&htim4, 0);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, (uint16_t)(g_toggle_pulse / 2)); /*  ���ö�ʱ���Ƚ�ֵ */
  HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_1);                                   /* ʹ�ܶ�ʱ��ͨ�� */
  ST1_EN(EN_ON);
}

/**
 * @brief       ���2 S�ͼӼ����˶�
 * @param       vo:���ٶ�;vt:ĩ�ٶ�;AcTime:����ʱ��;DeTime:����ʱ��;step:����;
 * @retval      ��
 */
void stepmotor2_move_rel(int32_t vo, int32_t vt, float AcTime, float DeTime, int32_t step)
{
  if (calc_speed(vo, vt, AcTime) == FALSE) /* ��������ٶε��ٶȺͲ��� */
    return;
  if (calc_speed(vt, vo, DeTime) == FALSE) /* ��������ٶε��ٶȺͲ��� */
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

  if (step >= (g_calc_t.decel_step + g_calc_t.accel_step)) /* ���ܲ������ڵ��ڼӼ��ٶȲ������ʱ���ſ���ʵ��������S�μӼ��� */
  {
    g_calc_t.step = step;
    g_calc_t.dec_point = g_calc_t.step - g_calc_t.decel_step; /* ��ʼ���ٵĲ��� */
  }
  else /* ���������Խ����㹻�ļӼ��� */
  {
    /* �������㲻�����˶���Ҫ��ǰ��������ٶȱ���ռ�ڴ��ͷţ��Ա�������ظ����� */
    myfree(SRAMIN, g_calc_t.accel_tab); /* �ͷż��ٶ��ٶȱ� */
    myfree(SRAMIN, g_calc_t.decel_tab); /* �ͷż��ٶ��ٶȱ� */
    printf("�������㣬�������ô���!\r\n");
    return;
  }
  g_calc_t.step_pos = 0;
  g_motor_sta = STATE_ACCEL; /* ���Ϊ����״̬ */

  g_calc_t.ptr = g_calc_t.accel_tab; /* �Ѽ��ٶε��ٶȱ�洢��ptr��� */
  g_toggle_pulse = (uint32_t)(T1_FREQ / (*g_calc_t.ptr));
  g_calc_t.ptr++;

  __HAL_TIM_SET_COUNTER(&htim1, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)(g_toggle_pulse / 2)); /*  ���ö�ʱ���Ƚ�ֵ */
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);                                   /* ʹ�ܶ�ʱ��ͨ�� */
  ST2_EN(EN_ON);
}
#endif

/**
 * @brief  ��ʱ���Ƚ��ж�
 * @param  htim����ʱ�����ָ��
 * @note   ��
 * @retval ��
 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef S_CURVE
  /**
   * @brief     :S �ͼӼ��������˶� �жϴ���
   * @attention :
   */
  volatile uint32_t Tim_Count = 0;
  volatile uint32_t tmp = 0;
  volatile float Tim_Pulse = 0;
  volatile static uint8_t i = 0;

  if (htim->Instance == TIM1)
  {
    i++;        /* ��ʱ���жϴ�������ֵ */
    if (i == 2) /* 2�Σ�˵���Ѿ����һ���������� */
    {
      i = 0;        /* ���㶨ʱ���жϴ�������ֵ */
      g_step_pos++; /* ��ǰλ�� */
      if ((g_motor_sta != STATE_IDLE) && (g_motor_sta != STATE_STOP))
      {
        g_calc_t.step_pos++;
      }
      switch (g_motor_sta)
      {
      case STATE_ACCEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr);        /* ���ٶȱ�õ�ÿһ���Ķ�ʱ������ֵ */
        g_calc_t.ptr++;                               /* ȡ�ٶȱ����һλ */
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);   /* ��תģʽC��Ҫ����2 */
        if (g_calc_t.step_pos >= g_calc_t.accel_step) /* �����ڼ��ٶβ����ͽ������� */
        {
          myfree(SRAMIN, g_calc_t.accel_tab); /* �˶���Ҫ�ͷ��ڴ� */
          g_motor_sta = STATE_AVESPEED;
        }
        break;
      case STATE_DECEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr); /* ���ٶȱ�õ�ÿһ���Ķ�ʱ������ֵ */
        g_calc_t.ptr++;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.step)
        {
          myfree(SRAMIN, g_calc_t.decel_tab); /* �˶���Ҫ�ͷ��ڴ� */
          g_motor_sta = STATE_STOP;
        }
        break;
      case STATE_AVESPEED:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / g_calc_t.vt;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.dec_point)
        {
          g_calc_t.ptr = g_calc_t.decel_tab; /* �����ٶε��ٶȱ�ֵ��ptr */
          g_motor_sta = STATE_DECEL;
        }
        break;
      case STATE_STOP:
        HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1); /* ������ӦPWMͨ�� */
        g_motor_sta = STATE_IDLE;
        break;
      case STATE_IDLE:
        break;
      }
    }
    /*  ���ñȽ�ֵ */
    Tim_Count = __HAL_TIM_GET_COUNTER(&htim1);
    tmp = 0xFFFF & (Tim_Count + g_toggle_pulse);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, tmp);
  }

  if (htim->Instance == TIM4)
  {
    i++;        /* ��ʱ���жϴ�������ֵ */
    if (i == 2) /* 2�Σ�˵���Ѿ����һ���������� */
    {
      i = 0;        /* ���㶨ʱ���жϴ�������ֵ */
      g_step_pos++; /* ��ǰλ�� */
      if ((g_motor_sta != STATE_IDLE) && (g_motor_sta != STATE_STOP))
      {
        g_calc_t.step_pos++;
      }
      switch (g_motor_sta)
      {
      case STATE_ACCEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr);        /* ���ٶȱ�õ�ÿһ���Ķ�ʱ������ֵ */
        g_calc_t.ptr++;                               /* ȡ�ٶȱ����һλ */
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);   /* ��תģʽC��Ҫ����2 */
        if (g_calc_t.step_pos >= g_calc_t.accel_step) /* �����ڼ��ٶβ����ͽ������� */
        {
          myfree(SRAMIN, g_calc_t.accel_tab); /* �˶���Ҫ�ͷ��ڴ� */
          g_motor_sta = STATE_AVESPEED;
        }
        break;
      case STATE_DECEL:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / (*g_calc_t.ptr); /* ���ٶȱ�õ�ÿһ���Ķ�ʱ������ֵ */
        g_calc_t.ptr++;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.step)
        {
          myfree(SRAMIN, g_calc_t.decel_tab); /* �˶���Ҫ�ͷ��ڴ� */
          g_motor_sta = STATE_STOP;
        }
        break;
      case STATE_AVESPEED:
        g_add_pulse_count++;
        Tim_Pulse = T1_FREQ / g_calc_t.vt;
        g_toggle_pulse = (uint16_t)(Tim_Pulse / 2);
        if (g_calc_t.step_pos >= g_calc_t.dec_point)
        {
          g_calc_t.ptr = g_calc_t.decel_tab; /* �����ٶε��ٶȱ�ֵ��ptr */
          g_motor_sta = STATE_DECEL;
        }
        break;
      case STATE_STOP:
        HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_1); /* ������ӦPWMͨ�� */
        g_motor_sta = STATE_IDLE;
        break;
      case STATE_IDLE:
        break;
      }
    }
    /*  ���ñȽ�ֵ */
    Tim_Count = __HAL_TIM_GET_COUNTER(&htim4);
    tmp = 0xFFFF & (Tim_Count + g_toggle_pulse);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tmp);
  }
#endif

  /**
   * @brief     :���ٿ��� + ��λ �жϴ���
   * @attention :
   */
  if (htim->Instance == TIM4)
  {
    static uint8_t count1 = 0;
    count1++;
    if (count1 % 2 == 0)
    {
      count1 = 0;
      g_run_flag1 = 1;
      g_stepper1.pulse_count--; /* ÿһ�������������-- */
      if (g_stepper1.dir == CW) /* ��ת */
      {
        g_stepper1.add_pulse_count++; /* ����λ��++ */
      }
      else
      {
        g_stepper1.add_pulse_count--; /* ����λ��-- */
      }
      if (g_stepper1.pulse_count <= 0) /* ������������0��ʱ�� ������Ҫ���͵������������ɣ�ֹͣ��ʱ����� */
      {
        printf("angle count: %d\r\n", (int)(g_stepper1.add_pulse_count * MAX_STEP_ANGLE)); /* ��ӡ�ۼ�ת���˶��ٽǶ� */
        stepper_stop(STEPPER_MOTOR_1);                                                     /* ֹͣ�ӿ�һ��� */
        ST1_EN(EN_OFF);
        g_run_flag1 = 0;
      }
    }

    /*��ȡ��ǰ����*/
    g_count_val1 = __HAL_TIM_GET_COUNTER(&htim4);
    /*���ñȽ���ֵ*/
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
      __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, (g_count_val1 + g_ccr_val1) % 0XFFFF);
  }

  if (htim->Instance == TIM1)
  {
    static uint8_t count2 = 0;
    count2++;
    if (count2 % 2 == 0)
    {
      count2 = 0;
      g_run_flag2 = 1;
      g_stepper2.pulse_count--; /* ÿһ�������������-- */
      if (g_stepper2.dir == CW) /* ��ת */
      {
        g_stepper2.add_pulse_count++; /* ����λ��++ */
      }
      else
      {
        g_stepper2.add_pulse_count--; /* ����λ��-- */
      }
      if (g_stepper2.pulse_count <= 0) /* ������������0��ʱ�� ������Ҫ���͵������������ɣ�ֹͣ��ʱ����� */
      {
        printf("angle count: %d\r\n", (int)(g_stepper2.add_pulse_count * MAX_STEP_ANGLE)); /* ��ӡ�ۼ�ת���˶��ٽǶ� */
        stepper_stop(STEPPER_MOTOR_1);                                                     /* ֹͣ�ӿ�һ��� */
        ST1_EN(EN_OFF);
        g_run_flag2 = 0;
      }
    }

    /*��ȡ��ǰ����*/
    g_count_val1 = __HAL_TIM_GET_COUNTER(&htim1);
    /*���ñȽ���ֵ*/
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (g_count_val2 + g_ccr_val2) % 0XFFFF);
  }
}
