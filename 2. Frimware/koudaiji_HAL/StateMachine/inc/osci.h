#ifndef __OSCI_H__
#define __OSCI_H__

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"

#define RESOLU 22.0f  //电压分辨率，1V电压对应ADC采集22个数
#define ZERO_VALUE 72.0f  //0电压偏移，电压输入为0时读数为73

#define VOLTAGE_COEFF (5.0f)

extern volatile bool adc_get_flag; 
extern uint32_t* adc_value;

extern arm_cfft_radix4_instance_f32 scfft;

void Osci_Show(void);

#endif


