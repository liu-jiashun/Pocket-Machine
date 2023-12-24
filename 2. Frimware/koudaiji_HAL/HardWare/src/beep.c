#include "beep.h"

GPIO_Struct BEEP_Pin;  //蜂鸣器控制引脚结构体变量

void Beep_Init(void)  //初始化蜂鸣器控制引脚
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    BEEP_Pin.GPIOx = GPIOG;  BEEP_Pin.GPIO_Pin = GPIO_PIN_14; //G14

    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = BEEP_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;  //下拉电阻使能
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;  //默认翻转速度
    HAL_GPIO_Init(BEEP_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(BEEP_Pin.GPIOx, BEEP_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

void Beep_Control(BEEP_State state)  //控制蜂鸣器开关
{
    HAL_GPIO_WritePin(BEEP_Pin.GPIOx, BEEP_Pin.GPIO_Pin, (GPIO_PinState)state);
}


