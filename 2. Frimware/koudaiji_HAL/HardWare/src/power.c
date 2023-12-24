#include "power.h"

GPIO_Struct Power_Pin; //��Դ���ƽӿڽṹ�����

/*��Դ���Ƴ�ʼ��*/
void Power_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Power_Pin.GPIOx = GPIOE;  Power_Pin.GPIO_Pin = GPIO_PIN_1;  //E1

    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = Power_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(Power_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(Power_Pin.GPIOx, Power_Pin.GPIO_Pin, GPIO_PIN_SET);
}

/*��Դ��һ�����ƶ˿ڣ��˿�Ϊ�͵�ƽʱ�Ż�ʹ�ܵ�Դ���*/
void Power_Control(Power_State state)
{
    HAL_GPIO_WritePin(Power_Pin.GPIOx, Power_Pin.GPIO_Pin, (GPIO_PinState)state);
}


