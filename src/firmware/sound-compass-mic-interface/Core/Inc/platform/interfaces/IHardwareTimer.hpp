/*
 * ITimer.hpp
 *
 *  Created on: Oct 16, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IHARDWARETIMER_HPP_
#define INC_PLATFORM_INTERFACES_IHARDWARETIMER_HPP_
//-------------------------------------------------------------------
// Module       : IHardwareTimer.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/INamed.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class IHardwareTimer: public platform::INamed
    {
    public:
        class ITimerListener
        {
        public:
            /**
             * @brief Timer event handler
             *
             * @param userData
             */
            virtual void onTimer(void *userData) = 0;
        };

        /**
         * @brief Stop the timer
         *
         * @return true success
         * @return false
         */
        virtual bool stop(void) = 0;

        /**
         * @brief Start the timer
         *
         * @return true success
         * @return false
         */
        virtual bool start(unsigned int ms) = 0;

        /**
         * @brief Restart the timer
         *
         * @return true success
         * @return false
         */
        virtual bool reset(void) = 0;

        /**
         * @brief Set the timer period
         *
         * @param freq_hz frequency in Hertz
         * @return true success
         * @return false
         */
        virtual bool setFreq(unsigned int freq_hz) = 0;

    };

} // namespace platform

#endif /* INC_PLATFORM_INTERFACES_IHARDWARETIMER_HPP_ */

