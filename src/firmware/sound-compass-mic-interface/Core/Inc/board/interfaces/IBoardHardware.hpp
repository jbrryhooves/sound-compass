/*
 * IBoardHardware.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_
#define INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdbool.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/IDiag.hpp"
#include "platform/interfaces/ILed.hpp"
#include "platform/interfaces/ISPI.hpp"
#include "platform/interfaces/ISAI.hpp"
#include "platform/interfaces/ITask.hpp"
#include "platform/interfaces/ITimer.hpp"
#include "platform/interfaces/IHardwareTimer.hpp"
#include "platform/interfaces/IMessageQueue.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace board
{
    class IBoardHardware
    {
    public:
        platform::IDiag *diag;
        platform::ILed *LED_debugGreen;
        platform::ILed *LED_debugOrange;
        platform::ILed *LED_debugRed;
        platform::ILed *LED1;
        platform::ILed *LED2;
        platform::ILed *LED3;
        platform::ILed *LED4;
        platform::ILed *LED5;
        platform::ILed *LED6;
        platform::ILed *LED7;
        platform::ILed *LED8;

        platform::IGPIO *GPIO_SPI_nCS1;
        platform::IGPIO *GPIO_SPI_nCS2;
        platform::IGPIO *GPIO_START;
        platform::IGPIO *GPIO_DRDY;
        platform::IGPIO *GPIO_PB1;
        platform::IGPIO *GPIO_PB2;

        platform::ISPI *SPI;
        platform::ISAI *SAI;
        platform::IHardwareTimer *HardwareTimer1;   // 48kHz audio sample rate
        platform::ITaskFactory *taskFactory;
        platform::ITimerFactory *timerFactory;
        platform::IMessageQueueFactory *messageQueueFactory;

    };
}

#endif /* INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_ */
