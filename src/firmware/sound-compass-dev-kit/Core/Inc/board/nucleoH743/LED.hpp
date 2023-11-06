/*
 * Led.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_BOARD_NUCLEOH743_LED_HPP_
#define INC_BOARD_NUCLEOH743_LED_HPP_
//-------------------------------------------------------------------
// Module       : Led.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdint.h>

// pin definitions, and HAL
#include "main.h"
//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ILed.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace board
{
    namespace nucleoH743
    {
        class LED: public platform::ILed
        {
        public:
            /**
             * @brief Initialise LED
             *
             * @param ledPin GPIO for LED
             * @return true Success
             * @return false
             */
            bool initialise(GPIO_TypeDef *gpioPort, uint16_t ledPin);

            // ILed
            void setLED(platform::ILed::LEDState ledState, uint32_t cycles);
            void toggleLED();

        private:

            GPIO_TypeDef *_gpioPort;
            uint16_t _ledPin;
            bool ledOn(bool on);

        };
    }
}

#endif /* INC_BOARD_NUCLEOH743_LED_HPP_ */

