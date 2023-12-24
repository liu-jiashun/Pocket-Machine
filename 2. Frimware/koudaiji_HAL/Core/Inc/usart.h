/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdbool.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

#define UART_BUFFER_SIZE 50  //�궨�崮�����ݽ��ջ������ĳ���

typedef struct Uart_Rcv  //�������ݽ��յĽṹ��
{
    uint8_t receive_buffer[UART_BUFFER_SIZE];  //�������ݽ��ջ�����
    bool receive_flag;  //�Ƿ���յ��µ�һ֡���ݵı�־
}Uart_Receive_Struct;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;

extern uint8_t uart5_receive_num;  //����5ʹ�ô������ݽ����жϺͿ����жϣ�������Ҫһ���������䵱ǰ����ջ������洢��λ��

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_UART4_Init(void);
void MX_UART5_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
