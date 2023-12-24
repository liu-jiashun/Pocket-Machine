#include "beep.h"

GPIO_Struct BEEP_Pin;  //�������������Žṹ�����

void Beep_Init(void)  //��ʼ����������������
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    BEEP_Pin.GPIOx = GPIOG;  BEEP_Pin.GPIO_Pin = GPIO_PIN_14; //G14

    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = BEEP_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;  //��������ʹ��
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;  //Ĭ�Ϸ�ת�ٶ�
    HAL_GPIO_Init(BEEP_Pin.GPIOx, &GPIO_InitStruct);

    HAL_GPIO_WritePin(BEEP_Pin.GPIOx, BEEP_Pin.GPIO_Pin, GPIO_PIN_RESET);
}

void Beep_Control(BEEP_State state)  //���Ʒ���������
{
    HAL_GPIO_WritePin(BEEP_Pin.GPIOx, BEEP_Pin.GPIO_Pin, (GPIO_PinState)state);
}


