/*
 * ILed.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_ILED_HPP_
#define INC_PLATFORM_INTERFACES_ILED_HPP_
//-------------------------------------------------------------------
// Module       : ILed.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdint.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class ILed
    {
    public:
        enum class LEDState
        {
            OFF, ON, //TOGGLE, FLASH_SLOW, FLASH_FAST, FLASH_RAPID, FLASH_DOUBLE,
        };

        /**
         * @brief Set LED state
         *
         * @param ledState
         * @param cycles Number of cycles (0=infinite)
         */
        virtual void setLED(LEDState ledState, uint32_t cycles) = 0;
        virtual void toggleLED() = 0;

    };

}

#endif /* INC_PLATFORM_INTERFACES_ILED_HPP_ */

