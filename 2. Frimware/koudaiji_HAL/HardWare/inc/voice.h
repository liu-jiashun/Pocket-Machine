#ifndef __VOICE_H__
#define __VOICE_H__

#include <stdint.h>
#include "usart.h"
#include "gpio.h"

typedef enum  //语音模块中语音文件的枚举
{
    DETECTED_R_SQUARE = 1,    
    DETECTED_G_SQUARE,    
    DETECTED_B_SQUARE,    
    DETECTED_Y_SQUARE,    
    DETECTED_R_CIRCLE,    
    DETECTED_G_CIRCLE,    
    DETECTED_B_CIRCLE,    
    DETECTED_Y_CIRCLE, 
    DETECTED_R_TRIANGLE,    
    DETECTED_G_TRIANGLE,    
    DETECTED_B_TRIANGLE,    
    DETECTED_Y_TRIANGLE, 
    DETECTED_R_PENTAGON,    
    DETECTED_G_PENTAGON,    
    DETECTED_B_PENTAGON,    
    DETECTED_Y_PENTAGON, 
    DETECTED_R_RHOMBOID,    
    DETECTED_G_RHOMBOID,    
    DETECTED_B_RHOMBOID,    
    DETECTED_Y_RHOMBOID, 
    DEVICE_STARTED,
    DEVICE_CHECKED,
    STATIC_START,
    SLOW_START,
    FAST_START,
    MANUAL_START,
    STOP_DETECT,
    LOCATION_TARGET_CORRD,
    PLASE_INPUT_TARGET_X,
    PLASE_INPUT_TARGET_Y,
    INPUT_ERROR,
    CAN_ONT_STOP,
    DEVICE_CHECKING,
} Voice_Enum;

extern Uart_Receive_Struct Voice_Receive;
extern volatile bool voice_is_busy;
extern GPIO_Struct Voice_Busy_Pin; 

void Voice_Init(void);
void Voice_Play(uint8_t voice);
void Voice_Busy_Scan(void);

#endif




