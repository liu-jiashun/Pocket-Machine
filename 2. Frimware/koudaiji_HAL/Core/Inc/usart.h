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

#define UART_BUFFER_SIZE 50  //宏定义串口数据接收缓冲区的长度

typedef struct Uart_Rcv  //串口数据接收的结构体
{
    uint8_t receive_buffer[UART_BUFFER_SIZE];  //串口数据接收缓冲区
    bool receive_flag;  //是否接收到新的一帧数据的标志
}Uart_Receive_Struct;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;

extern uint8_t uart5_receive_num;  //串口5使用串口数据接收中断和空闲中断，所以需要一个变量记忆当前向接收缓冲区存储的位置

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
