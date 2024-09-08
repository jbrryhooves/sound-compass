//-------------------------------------------------------------------
// Module       : SPI.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include <platform/STM32_HAL/GPIO.hpp>
#include "util/assert.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platform::stm32hal::GPIO::initialise(GPIO_TypeDef *gpioPort, uint16_t pin)
{
    _gpioPort = gpioPort;
    _pin = pin;

    return true;
}


// IGPIO
void platform::stm32hal::GPIO::set(GPIOState state)
{
    HAL_GPIO_WritePin(_gpioPort, _pin, (GPIO_PinState) state);
}

void platform::stm32hal::GPIO::toggle()
{
    if (this->read() == GPIOState::Low)
    {
        this->set(GPIOState::High);
    }
    else
    {
        this->set(GPIOState::Low);
    }
}

platform::IGPIO::GPIOState platform::stm32hal::GPIO::read()
{
    return (GPIOState) HAL_GPIO_ReadPin(_gpioPort, _pin);
}

void platform::stm32hal::GPIO::registerCallback(interruptCallback_t *callbackMethod)
{
    _callback = callbackMethod;
}

/// @brief Called by the global interrupt handler
void platform::stm32hal::GPIO::interruptHandler()
{
    if(_callback != nullptr)
    {
        (*_callback)();
    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
