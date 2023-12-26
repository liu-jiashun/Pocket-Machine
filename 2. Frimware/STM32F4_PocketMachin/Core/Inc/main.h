/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED7_Pin GPIO_PIN_2
#define LED7_GPIO_Port GPIOE
#define LED6_Pin GPIO_PIN_3
#define LED6_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_4
#define LED5_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_6
#define LED3_GPIO_Port GPIOE
#define MATRIX_KEY_R1_Pin GPIO_PIN_1
#define MATRIX_KEY_R1_GPIO_Port GPIOF
#define MATRIX_KEY_C3_Pin GPIO_PIN_3
#define MATRIX_KEY_C3_GPIO_Port GPIOF
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_7
#define LED1_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOF
#define MINI_LED0_Pin GPIO_PIN_9
#define MINI_LED0_GPIO_Port GPIOF
#define MINI_LED1_Pin GPIO_PIN_10
#define MINI_LED1_GPIO_Port GPIOF
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOC
#define TOUCH_KEY_A_Pin GPIO_PIN_0
#define TOUCH_KEY_A_GPIO_Port GPIOA
#define MATRIX_KEY_C2_Pin GPIO_PIN_1
#define MATRIX_KEY_C2_GPIO_Port GPIOB
#define MATRIX_KEY_C4_Pin GPIO_PIN_11
#define MATRIX_KEY_C4_GPIO_Port GPIOF
#define MATRIX_KEY_R2_Pin GPIO_PIN_12
#define MATRIX_KEY_R2_GPIO_Port GPIOF
#define MATRIX_KEY_R4_Pin GPIO_PIN_13
#define MATRIX_KEY_R4_GPIO_Port GPIOF
#define RELAY_Pin GPIO_PIN_15
#define RELAY_GPIO_Port GPIOF
#define TIRPOD_Y_EN_Pin GPIO_PIN_7
#define TIRPOD_Y_EN_GPIO_Port GPIOE
#define TIRPOD_Y_STEP_Pin GPIO_PIN_9
#define TIRPOD_Y_STEP_GPIO_Port GPIOE
#define TIRPOD_Y_LIMIT_Pin GPIO_PIN_11
#define TIRPOD_Y_LIMIT_GPIO_Port GPIOE
#define MAX485_MODE_Pin GPIO_PIN_15
#define MAX485_MODE_GPIO_Port GPIOB
#define MATRIX_KEY_C1_Pin GPIO_PIN_9
#define MATRIX_KEY_C1_GPIO_Port GPIOD
#define MATRIX_KEY_R3_Pin GPIO_PIN_12
#define MATRIX_KEY_R3_GPIO_Port GPIOD
#define MATRIX_KEY_R5_Pin GPIO_PIN_14
#define MATRIX_KEY_R5_GPIO_Port GPIOD
#define TOUCH_KEY_B_Pin GPIO_PIN_6
#define TOUCH_KEY_B_GPIO_Port GPIOG
#define TOUCH_KEY_C_Pin GPIO_PIN_7
#define TOUCH_KEY_C_GPIO_Port GPIOG
#define TOUCH_KEY_D_Pin GPIO_PIN_8
#define TOUCH_KEY_D_GPIO_Port GPIOG
#define VISION_TX_Pin GPIO_PIN_9
#define VISION_TX_GPIO_Port GPIOA
#define VISION_RX_Pin GPIO_PIN_10
#define VISION_RX_GPIO_Port GPIOA
#define TIRPOD_X_EN_Pin GPIO_PIN_15
#define TIRPOD_X_EN_GPIO_Port GPIOA
#define VOICE_TX_Pin GPIO_PIN_10
#define VOICE_TX_GPIO_Port GPIOC
#define VOICE_RX_Pin GPIO_PIN_11
#define VOICE_RX_GPIO_Port GPIOC
#define MAX485_TX_Pin GPIO_PIN_12
#define MAX485_TX_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_0
#define LCD_RST_GPIO_Port GPIOD
#define TIRPOD_X_DIR_Pin GPIO_PIN_1
#define TIRPOD_X_DIR_GPIO_Port GPIOD
#define MAX485_RX_Pin GPIO_PIN_2
#define MAX485_RX_GPIO_Port GPIOD
#define TOUCH_KEY_RIGHT_Pin GPIO_PIN_3
#define TOUCH_KEY_RIGHT_GPIO_Port GPIOD
#define TOUCH_KEY_DOWN_Pin GPIO_PIN_6
#define TOUCH_KEY_DOWN_GPIO_Port GPIOD
#define TOUCH_KEY_LEFT_Pin GPIO_PIN_10
#define TOUCH_KEY_LEFT_GPIO_Port GPIOG
#define TOUCH_KEY_UP_Pin GPIO_PIN_11
#define TOUCH_KEY_UP_GPIO_Port GPIOG
#define INFARED_SEND_Pin GPIO_PIN_12
#define INFARED_SEND_GPIO_Port GPIOG
#define BEEP_Pin GPIO_PIN_14
#define BEEP_GPIO_Port GPIOG
#define TIRPOD_X_STEP_Pin GPIO_PIN_15
#define TIRPOD_X_STEP_GPIO_Port GPIOG
#define TIRPOD_X_LIMIT_Pin GPIO_PIN_6
#define TIRPOD_X_LIMIT_GPIO_Port GPIOB
#define TIRPOD_Y_DIR_Pin GPIO_PIN_0
#define TIRPOD_Y_DIR_GPIO_Port GPIOE
#define VOICE_BUSY_Pin GPIO_PIN_1
#define VOICE_BUSY_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define ON  1
#define OFF 0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
