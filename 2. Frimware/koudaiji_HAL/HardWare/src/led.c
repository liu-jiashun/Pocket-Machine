#include "led.h"

GPIO_Struct LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7;  //LED灯接口结构体变量

/*LED灯接口初始化，推挽输出、无上下拉、默认翻转速度*/
void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LED0.GPIOx = GPIOF;  LED0.GPIO_Pin = GPIO_PIN_8;
    LED1.GPIOx = GPIOF;  LED1.GPIO_Pin = GPIO_PIN_7;
    LED2.GPIOx = GPIOF;  LED2.GPIO_Pin = GPIO_PIN_6;
    LED3.GPIOx = GPIOE;  LED3.GPIO_Pin = GPIO_PIN_6;
    LED4.GPIOx = GPIOE;  LED4.GPIO_Pin = GPIO_PIN_5;
    LED5.GPIOx = GPIOE;  LED5.GPIO_Pin = GPIO_PIN_4;
    LED6.GPIOx = GPIOE;  LED6.GPIO_Pin = GPIO_PIN_3;
    LED7.GPIOx = GPIOE;  LED7.GPIO_Pin = GPIO_PIN_2;

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LED0.GPIOx, LED0.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1.GPIOx, LED1.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2.GPIOx, LED2.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED3.GPIOx, LED3.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED4.GPIOx, LED4.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED5.GPIOx, LED5.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED6.GPIOx, LED6.GPIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED7.GPIOx, LED7.GPIO_Pin, GPIO_PIN_SET);
}

/*LED灯状态控制*/
void Led_Control(GPIO_Struct_P led, LED_State state)
{
    HAL_GPIO_WritePin(led->GPIOx, led->GPIO_Pin, (GPIO_PinState)state);
}

//Led_Control(&LED2, LED_ON); //LED2 开

