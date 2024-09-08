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
#include "platform/interfaces/IHardwareTimer.hpp"
#include "platform/interfaces/IHardwarePWM.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::stm32hal
{

    class HardwareTimer: public platform::IHardwareTimer, public platform::IHardwarePWM
    {
    public:
        /**
         * @brief Initialise GPIO
         *
         * @param ledPin GPIO for LED
         * @return true Success
         * @return false
         */
        bool initialise(TIM_HandleTypeDef *timer, uint8_t channel);

        // IHardwareTimer
        bool stop(void);
        bool start(unsigned int ms);
        bool reset(void);
        bool setFreq(unsigned int freq_hz);

        // IHardwarePWM
        bool stop(void);
        bool start(unsigned int ms);
        bool setDuty(float percent);
        bool setFreq(unsigned int freq_hz);

    private:

        TIM_HandleTypeDef *_timerHandle;
        uint16_t _channel;

    };

}

#endif /* INC_BOARD_NUCLEOH743_LED_HPP_ */

