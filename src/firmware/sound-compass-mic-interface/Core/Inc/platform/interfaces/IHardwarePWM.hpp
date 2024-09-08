/*
 * ITimer.hpp
 *
 *  Created on: Oct 16, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IHARDWAREPWM_HPP_
#define INC_PLATFORM_INTERFACES_IHARDWAREPWM_HPP_
//-------------------------------------------------------------------
// Module       : IHardwarePWM.hpp
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
    class IHardwarePWM: public platform::INamed
    {
    public:


        /**
         * @brief Stop the timer
         *
         * @return true success
         * @return false
         */
        virtual bool stopPWM(void) = 0;

        /**
         * @brief Start the timer
         *
         * @return true success
         * @return false
         */
        virtual bool startPWM(unsigned int ms) = 0;

        /**
         * @brief Set the timer period
         *
         * @param ms period in milliseconds
         * @return true success
         * @return false
         */
        virtual bool setDuty(float percent) = 0;
        

    };

} // namespace platform

#endif /* INC_PLATFORM_INTERFACES_IHARDWARETIMER_HPP_ */

