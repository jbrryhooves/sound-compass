
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
// TODO: change CubeMX to not generaate init code, and put it in here instead.
extern UART_HandleTypeDef huart3;

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
bool board::boardNucleoH743::initialise()
{
    if(!_diagSTM32.initialise(&huart3))
    {
        return false;
    }
    diag = &_diagSTM32;

    // init the LED and all the hardware...
    if(!_LED_debugGreen.initialise(LD1_GPIO_Port, LD1_Pin))
    {
        // print error
        return false;
    }
    LED_debugGreen = &_LED_debugGreen;

    diag->info("NucleoHW", "Board init done\n");

    return true;
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------


