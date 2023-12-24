#include "voice.h"
#include "stdbool.h"

volatile bool voice_is_busy = false;  //����ģ��æ���
Uart_Receive_Struct Voice_Receive;
GPIO_Struct Voice_Busy_Pin; 

/*����ģ��æ���ӿڳ�ʼ��*/
void Voice_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Voice_Busy_Pin.GPIOx = GPIOB;  Voice_Busy_Pin.GPIO_Pin = GPIO_PIN_9;

    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitStruct.Pin = Voice_Busy_Pin.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(Voice_Busy_Pin.GPIOx, &GPIO_InitStruct);
}

/*
*���ܣ�����ָ���������ļ�
*������voice�������ļ�
*/
void Voice_Play(uint8_t voice)
{
    uint8_t data[8] = {0x00,0x7E,0x05,0x41,0x00,0xff,0xff,0xEF};
    data[5] = voice;
    data[6] = data[2] ^ data[3] ^ data[4] ^ data[5]; //���У��λ

    HAL_UART_Transmit(&huart4, data, 8, HAL_MAX_DELAY);
}

/*����ģ��æ����ź�ɨ��*/
void Voice_Busy_Scan(void)
{
    voice_is_busy = (bool)HAL_GPIO_ReadPin(Voice_Busy_Pin.GPIOx, Voice_Busy_Pin.GPIO_Pin);
} 


