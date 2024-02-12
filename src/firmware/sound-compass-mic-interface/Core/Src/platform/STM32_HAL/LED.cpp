//-------------------------------------------------------------------
// Module       : LED.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdint.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/STM32_HAL/LED.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platform::stm32hal::LED::initialise(GPIO_TypeDef *gpioPort, uint16_t ledPin)
{
    _gpioPort = gpioPort;
    _ledPin = ledPin;

    // consider moving init code here, instead of in main.c

    return true;
}

void platform::stm32hal::LED::setLED(platform::ILed::LEDState ledState, uint32_t cycles)
{
    switch (ledState)
    {
        case platform::ILed::LEDState::OFF:
            ledOn(false);
            break;
        case platform::ILed::LEDState::ON:
            ledOn(true);
            break;
        default:

            break;
    }
}

void platform::stm32hal::LED::toggleLED()
{
    HAL_GPIO_TogglePin(_gpioPort, _ledPin);
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

bool platform::stm32hal::LED::ledOn(bool on)
{
    HAL_GPIO_WritePin(_gpioPort, _ledPin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return true;
}
