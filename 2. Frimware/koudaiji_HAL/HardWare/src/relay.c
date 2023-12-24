#include "relay.h"

GPIO_Struct Relay_Pin;  //继电器控制接口结构体

/*继电器控制端口初始化，（继电器控制激光笔的通断）*/
void Relay_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Relay_Pin.GPIOx = GPIOF;  Relay_Pin.GPIO_Pin = GPIO_PIN_15;

    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitStruct.Pin = Relay_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(Relay_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(Relay_Pin.GPIOx, Relay_Pin.GPIO_Pin, GPIO_PIN_SET);
}

void Relay_Control(Relay_State state)
{
    HAL_GPIO_WritePin(Relay_Pin.GPIOx, Relay_Pin.GPIO_Pin, (GPIO_PinState)state);
}


