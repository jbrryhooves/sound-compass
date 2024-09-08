/*
 * Led.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_STM32_HAL_GPIO_HPP_
#define INC_PLATFORM_STM32_HAL_GPIO_HPP_
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
#include "platform/interfaces/IGPIO.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::stm32hal
{

    class GPIO: public platform::IGPIO
    {
    public:
        /**
         * @brief Initialise GPIO
         *
         * @param ledPin GPIO for LED
         * @return true Success
         * @return false
         */
        bool initialise(GPIO_TypeDef *gpioPort, uint16_t pin);

        // IGPIO
        void set(GPIOState state);
        void toggle();
        GPIOState read();
        void registerCallback(interruptCallback_t *callback);
        void interruptHandler();


    private:

        GPIO_TypeDef *_gpioPort;
        uint16_t _pin;
        interruptCallback_t *_callback;

    };

}

#endif /* INC_BOARD_NUCLEOH743_LED_HPP_ */

