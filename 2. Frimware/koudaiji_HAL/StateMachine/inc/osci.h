#ifndef __OSCI_H__
#define __OSCI_H__

#include <stdint.h>
#include <stdbool.h>
#include "arm_math.h"

#define RESOLU 22.0f  //��ѹ�ֱ��ʣ�1V��ѹ��ӦADC�ɼ�22����
#define ZERO_VALUE 72.0f  //0��ѹƫ�ƣ���ѹ����Ϊ0ʱ����Ϊ73

#define VOLTAGE_COEFF (5.0f)

extern volatile bool adc_get_flag; 
extern uint32_t* adc_value;

extern arm_cfft_radix4_instance_f32 scfft;

void Osci_Show(void);

#endif


