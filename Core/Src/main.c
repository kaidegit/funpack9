/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dfsdm.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdmmc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "custom_env_sensors.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_DFSDM1_Init();
    MX_I2C3_Init();
    MX_RTC_Init();
    MX_SDMMC1_SD_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();
    MX_SPI3_Init();
    MX_TIM2_Init();
    MX_TIM6_Init();
    MX_TIM7_Init();
    MX_FATFS_Init();
    MX_USB_DEVICE_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    char ch[100];
    HAL_UART_Transmit(&huart1, "HelloWorld\n", sizeof("HelloWorld\n"), 0xff);
    if (f_mount(&SDFatFS, (TCHAR const *) SDPath, 0) == FR_OK) {
        HAL_UART_Transmit(&huart1, "sd card is ok\n", sizeof("sd card is ok\n"), 0xff);
    } else {
        HAL_UART_Transmit(&huart1, "sd card is not ok\n", sizeof("sd card is not ok\n"), 0xff);
    }
//    FRESULT fr = f_open(&SDFile,"aaaaa.txt",FA_OPEN_ALWAYS | FA_WRITE);
//    if (fr == FR_OK){
//        HAL_UART_Transmit(&huart1, "file opened successfully\n", sizeof("file opened successfully\n"), 0xff);
//        UINT num;
//        fr = f_write(&SDFile,"bbbb", sizeof("bbbb"),&num);
//        sprintf(ch,"witten %d characters\n",num);
//        HAL_UART_Transmit(&huart1, ch, strlen(ch), 0xff);
//        fr = f_close(&SDFile);
//    }else{
//        HAL_UART_Transmit(&huart1, "file opened not successfully\n", sizeof("file opened not successfully\n"), 0xff);
//    }
    FRESULT fr = f_open(&SDFile, "data.csv", FA_OPEN_ALWAYS | FA_WRITE);
    if (fr == FR_OK) {
        HAL_UART_Transmit(&huart1, "file opened successfully\n", sizeof("file opened successfully\n"), 0xff);
        UINT num;
        sprintf(ch, "temperature,humidity,pressure\n");
        fr = f_write(&SDFile, ch, strlen(ch), &num);
        fr = f_close(&SDFile);
    } else {
        HAL_UART_Transmit(&huart1, "file opened not successfully\n", sizeof("file opened not successfully\n"), 0xff);
    }
    fr = f_close(&SDFile);

    CUSTOM_ENV_SENSOR_Init(CUSTOM_HTS221_0, ENV_TEMPERATURE);
    CUSTOM_ENV_SENSOR_Init(CUSTOM_HTS221_0, ENV_HUMIDITY);
    CUSTOM_ENV_SENSOR_Init(CUSTOM_LPS22HH_0, ENV_PRESSURE);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (1) {
//        UINT num;
//        FRESULT fr = f_open(&SDFile, "testBat.txt", FA_OPEN_APPEND | FA_WRITE);
//        sprintf(ch,"%d",i);
//        fr = f_write(&SDFile, ch, strlen(ch), &num);
//        HAL_UART_Transmit(&huart1, ch, strlen(ch), 0xff);
//        fr = f_close(&SDFile);

        float temperature;
        CUSTOM_ENV_SENSOR_GetValue(CUSTOM_HTS221_0, ENV_TEMPERATURE, &temperature);
        float humidity;
        CUSTOM_ENV_SENSOR_GetValue(CUSTOM_HTS221_0, ENV_HUMIDITY, &humidity);
        float pressure;
        CUSTOM_ENV_SENSOR_GetValue(CUSTOM_LPS22HH_0, ENV_PRESSURE, &pressure);
        sprintf(ch, "%f,%f,%f\n", temperature, humidity, pressure);
        HAL_UART_Transmit(&huart1, ch, strlen(ch), 0xff);

        UINT num;
        FRESULT fr = f_open(&SDFile, "data.csv", FA_OPEN_APPEND | FA_WRITE);
        fr = f_write(&SDFile, ch, strlen(ch), &num);
        fr = f_close(&SDFile);
        HAL_Delay(5000);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
#pragma clang diagnostic pop
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK) {
        Error_Handler();
    }
    /** Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE
                                       | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 60;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_USART1
                                         | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_I2C3
                                         | RCC_PERIPHCLK_DFSDM1 | RCC_PERIPHCLK_USB
                                         | RCC_PERIPHCLK_SDMMC1 | RCC_PERIPHCLK_ADC;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLP;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 5;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 96;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV4;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
