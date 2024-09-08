//-------------------------------------------------------------------
// Module       : nucleo-h7432.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include "main.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "board/nucleoH743/nucleo-h7432.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

// declared in main.c
// TODO: change CubeMX to not generate init code, and put it in here instead.
extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SAI_HandleTypeDef hsai_BlockA1;
extern SAI_HandleTypeDef hsai_BlockB1;
//extern DMA_HandleTypeDef hdma_sai1_a;
//extern DMA_HandleTypeDef hdma_sai1_b;


// keep a file scoped reference to the board instance for the c-based interrupt handler to access
static board::boardNucleoH743 *_instance;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
bool board::boardNucleoH743::initialise()
{


    if (!_diagSTM32.initialise(&huart3))
    {
        return false;
    }
    diag = &_diagSTM32;

    // init the LED and all the hardware...
    if (!_LED_debugGreen.initialise(LD1_GPIO_Port, LD1_Pin))
    {
        // print error
        return false;
    }
    LED_debugGreen = &_LED_debugGreen;

    if (!_LED_debugOrange.initialise(LD2_GPIO_Port, LD2_Pin))
    {
        // print error
        return false;
    }
    LED_debugOrange = &_LED_debugOrange;

    if (!_LED_debugRed.initialise(LD3_GPIO_Port, LD3_Pin))
    {
        // print error
        return false;
    }
    LED_debugRed = &_LED_debugRed;



    if (!_LED1.initialise(GPIO_OUT_LED1_GPIO_Port, GPIO_OUT_LED1_Pin))
    {
        // print error
        return false;
    }
    LED1 = &_LED1;

    if (!_LED2.initialise(GPIO_OUT_LED2_GPIO_Port, GPIO_OUT_LED2_Pin))
    {
        // print error
        return false;
    }
    LED2 = &_LED2;

    if (!_LED3.initialise(GPIO_OUT_LED3_GPIO_Port, GPIO_OUT_LED3_Pin))
    {
        // print error
        return false;
    }
    LED3 = &_LED3;

    if (!_LED4.initialise(GPIO_OUT_LED4_GPIO_Port, GPIO_OUT_LED4_Pin))
    {
        // print error
        return false;
    }
    LED4 = &_LED4;

    if (!_LED5.initialise(GPIO_OUT_LED5_GPIO_Port, GPIO_OUT_LED5_Pin))
    {
        // print error
        return false;
    }
    LED5 = &_LED5;

    if (!_LED6.initialise(GPIO_OUT_LED6_GPIO_Port, GPIO_OUT_LED6_Pin))
    {
        // print error
        return false;
    }
    LED6 = &_LED6;

    if (!_LED7.initialise(GPIO_OUT_LED7_GPIO_Port, GPIO_OUT_LED7_Pin))
    {
        // print error
        return false;
    }
    LED7 = &_LED7;

    if (!_LED8.initialise(GPIO_OUT_LED8_GPIO_Port, GPIO_OUT_LED8_Pin))
    {
        // print error
        return false;
    }
    LED8 = &_LED8;

    if (!_GPIO_SPI_nCS1.initialise(GPIO_OUT_SPI1_nCS1_GPIO_Port, GPIO_OUT_SPI1_nCS1_Pin))
    {
        // print error
        return false;
    }
    GPIO_SPI_nCS1 = &_GPIO_SPI_nCS1;

    if (!_GPIO_SPI_nCS2.initialise(GPIO_OUT_SPI1_nCS2_GPIO_Port, GPIO_OUT_SPI1_nCS2_Pin))
    {
        // print error
        return false;
    }
    GPIO_SPI_nCS2 = &_GPIO_SPI_nCS2;

//    if (!_GPIO_START.initialise(GPIO_OUT_nSTART_GPIO_Port, GPIO_OUT_nSTART_Pin))
//    {
//        // print error
//        return false;
//    }
//    GPIO_START = &_GPIO_START;

    if (!_GPIO_DRDY.initialise(GPIO_EXTI0_DRDY_GPIO_Port, GPIO_EXTI0_DRDY_Pin))
    {
        // print error
        return false;
    }
    GPIO_DRDY = &_GPIO_DRDY;
    

    if (!_GPIO_PB1.initialise(GPIO_EXTI1_PushButton1_GPIO_Port, GPIO_EXTI1_PushButton1_Pin))
    {
        // print error
        return false;
    }
    GPIO_PB1 = &_GPIO_PB1;

    if (!_GPIO_PB2.initialise(GPIO_EXTI1_PushButton2_GPIO_Port, GPIO_EXTI1_PushButton2_Pin))
    {
        // print error
        return false;
    }
    GPIO_PB2 = &_GPIO_PB2;


    if (!_SPI.initialise(&hspi1, &hdma_spi1_rx, &hdma_spi1_tx))
    {
        // print error
        return false;
    }
    SPI = &_SPI;

//    if (!_SAI.initialise(&hsai_BlockA1, &hsai_BlockB1, &hdma_sai1_a, &hdma_sai1_b))
    if (!_SAI.initialise(&hsai_BlockA1, &hsai_BlockB1, NULL, NULL))
    {
        // print error
        return false;
    }
    SAI = &_SAI;


    if (!_taskFreeRTOSFactory.initialise())
    {
        // print error
        return false;
    }
    taskFactory = &_taskFreeRTOSFactory;

    if (!_messageQueueFreeRTOSFactory.initialise())
    {
        // print error
        return false;
    }
    messageQueueFactory = &_messageQueueFreeRTOSFactory;

    if (!_timerFreeRTOSFactory.initialise())
    {
        // print error
        return false;
    }
    timerFactory = &_timerFreeRTOSFactory;

    diag->info("NucleoHW", "Board init done\n");

    _instance = this;
    return true;
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------


/// @brief Implement STM32HAL callbacks
/// @param GPIO_Pin 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (_instance != nullptr)
    {
        switch (GPIO_Pin)
        {
            case GPIO_EXTI0_DRDY_Pin:
                _instance->GPIO_DRDY->interruptHandler();
                break;
            case GPIO_EXTI1_PushButton1_Pin:
                _instance->GPIO_PB1->interruptHandler();
                break;
            case GPIO_EXTI1_PushButton2_Pin:
                _instance->GPIO_PB2->interruptHandler();
                break;

            default:
                break;
        }
    }
    // this worksinterruptHandler()
}
