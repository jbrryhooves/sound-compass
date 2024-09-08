/*
 * ILed.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IGPIO_HPP_
#define INC_PLATFORM_INTERFACES_IGPIO_HPP_
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
    class IGPIO
    {
    public:
        // class IGPIOListener
        // {
        // public:
        //     virtual void onEdge() = 0;
        // };

        /// @brief callback method signature
        using interruptCallback_t = void (void);

        enum class GPIOState
        {
            Low = 0,
            High,
        };

        /**
         * @brief Set GPIO state
         *
         * @param ledState
         */
        virtual void set(GPIOState state) = 0;
        virtual void toggle() = 0;
        virtual GPIOState read() = 0;
        virtual void registerCallback(interruptCallback_t *listener) = 0;

        /// @brief Called by the global interrupt handler
        virtual void interruptHandler() = 0;


    };

}

#endif /* INC_PLATFORM_INTERFACES_ILED_HPP_ */

