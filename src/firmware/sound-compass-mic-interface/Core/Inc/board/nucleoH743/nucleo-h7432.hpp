/*
 * nucleo-h7432.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_BOARD_NUCLEO_H7432_HPP_
#define INC_BOARD_NUCLEO_H7432_HPP_
//-------------------------------------------------------------------
// Module       : nucleo-h7432.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "board/interfaces/IBoardHardware.hpp"
#include <platform/STM32_HAL/DiagSTM32H7.hpp>
#include <platform/STM32_HAL/SPI.hpp>
#include <platform/STM32_HAL/SAI.hpp>
#include "platform/STM32_HAL/LED.hpp"
#include "platform/STM32_HAL/GPIO.hpp"
#include "platform/FreeRTOS/TaskFreeRTOS.hpp"
#include "platform/FreeRTOS/MessageQueueFreeRTOS.hpp"
#include "platform/FreeRTOS/TimerFreeRTOS.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace board
{
    class boardNucleoH743: public board::IBoardHardware
    {
    public:
        bool initialise();


    private:
        platform::stm32hal::DiagSTM32 _diagSTM32;
        platform::FreeRTOS::TaskFreeRTOSFactory _taskFreeRTOSFactory;
        platform::FreeRTOS::TimerFreeRTOSFactory _timerFreeRTOSFactory;
        platform::FreeRTOS::MessageQueueFreeRTOSFactory _messageQueueFreeRTOSFactory;

        platform::stm32hal::GPIO _GPIO_SPI_nCS1;
        platform::stm32hal::GPIO _GPIO_SPI_nCS2;
        platform::stm32hal::GPIO _GPIO_START;
        platform::stm32hal::GPIO _GPIO_DRDY;
        platform::stm32hal::GPIO _GPIO_PB1;
        platform::stm32hal::GPIO _GPIO_PB2;
        platform::stm32hal::SAI  _SAI;

        platform::stm32hal::LED _LED1;
        platform::stm32hal::LED _LED2;
        platform::stm32hal::LED _LED3;
        platform::stm32hal::LED _LED4;
        platform::stm32hal::LED _LED5;
        platform::stm32hal::LED _LED6;
        platform::stm32hal::LED _LED7;
        platform::stm32hal::LED _LED8;

        platform::stm32hal::LED _LED_debugGreen;
        platform::stm32hal::LED _LED_debugOrange;
        platform::stm32hal::LED _LED_debugRed;
        platform::stm32hal::SPI _SPI;

//        platform::IHardwareTimer *HardwareTimer1;   // 48kHz audio sample rate
    };
}

#endif /* INC_BOARD_NUCLEO_H7432_HPP_ */

