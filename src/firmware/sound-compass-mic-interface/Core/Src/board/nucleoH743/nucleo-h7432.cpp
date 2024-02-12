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

    if (!_SPI.initialise(&hspi1, &hdma_spi1_rx, &hdma_spi1_tx))
    {
        // print error
        return false;
    }
    SPI = &_SPI;




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

    return true;
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

