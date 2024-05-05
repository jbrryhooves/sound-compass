/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define GPIO_OUT_LED4_Pin GPIO_PIN_6
#define GPIO_OUT_LED4_GPIO_Port GPIOE
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define GPIO_EXTI0_DRDY_Pin GPIO_PIN_0
#define GPIO_EXTI0_DRDY_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define GPIO_OUT_nSTART_Pin GPIO_PIN_2
#define GPIO_OUT_nSTART_GPIO_Port GPIOB
#define GPIO_OUT_LED5_Pin GPIO_PIN_0
#define GPIO_OUT_LED5_GPIO_Port GPIOG
#define GPIO_OUT_LED6_Pin GPIO_PIN_1
#define GPIO_OUT_LED6_GPIO_Port GPIOG
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define GPIO_OUT_SPI1_nCS2_Pin GPIO_PIN_13
#define GPIO_OUT_SPI1_nCS2_GPIO_Port GPIOD
#define USB_OTG_FS_OVCR_Pin GPIO_PIN_7
#define USB_OTG_FS_OVCR_GPIO_Port GPIOG
#define GPIO_EXTI1_PushButton1_Pin GPIO_PIN_10
#define GPIO_EXTI1_PushButton1_GPIO_Port GPIOC
#define GPIO_EXTI1_PushButton2_Pin GPIO_PIN_11
#define GPIO_EXTI1_PushButton2_GPIO_Port GPIOC
#define GPIO_OUT_LED7_Pin GPIO_PIN_0
#define GPIO_OUT_LED7_GPIO_Port GPIOD
#define GPIO_OUT_LED8_Pin GPIO_PIN_1
#define GPIO_OUT_LED8_GPIO_Port GPIOD
#define GPIO_OUT_LED3_Pin GPIO_PIN_9
#define GPIO_OUT_LED3_GPIO_Port GPIOG
#define GPIO_OUT_LED1_Pin GPIO_PIN_12
#define GPIO_OUT_LED1_GPIO_Port GPIOG
#define GPIO_OUT_LED2_Pin GPIO_PIN_15
#define GPIO_OUT_LED2_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_1
#define LD2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
