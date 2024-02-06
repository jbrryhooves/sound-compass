/*
 * ITimer.hpp
 *
 *  Created on: Oct 16, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_ITIMER_HPP_
#define INC_PLATFORM_INTERFACES_ITIMER_HPP_
//-------------------------------------------------------------------
// Module       : ITimer.hpp
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
    class ITimer: public platform::INamed
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
         * @brief Stop the timer from ISR
         *
         * @return true success
         * @return false
         */
        virtual bool stopFromISR(void) = 0;

        /**
         * @brief Start the timer
         *
         * @return true success
         * @return false
         */
        virtual bool start(unsigned int ms) = 0;

        /**
         * @brief Start the timer from ISR
         *
         * @return true success
         * @return false
         */
        virtual bool startFromISR(void) = 0;

        /**
         * @brief Restart the timer
         *
         * @return true success
         * @return false
         */
        virtual bool reset(void) = 0;

        /**
         * @brief Restart timer from ISR
         *
         * @return true success
         * @return false
         */
        virtual bool resetFromISR(void) = 0;

        /**
         * @brief Set the timer period
         *
         * @param ms period in milliseconds
         * @return true success
         * @return false
         */
        virtual bool setPeriod(unsigned int ms) = 0;

    };

    class ITimerFactory
    {
    public:
        /**
         * @brief Create a Timer
         *
         * @param name Name of timer
         * @param ms timer interval in milliseconds
         * @param autoRestart
         * @param listener Timer listener
         * @param userData User defined data
         * @return ITimer* Handle to timer
         */
        virtual ITimer*
        createTimer(const char *name, unsigned int ms, bool autoRestart, ITimer::ITimerListener *listener, void *userData) = 0;
        virtual ITimer* createTimerStatic(const char *timerName, void *timerControlBlock, unsigned int controlBlockSize, bool periodic, ITimer::ITimerListener *listener,
                void *userData) = 0;

        /**
         * @brief Destroy timer
         *
         * @param timer
         * @return true
         * @return false
         */
        virtual bool destroyTimer(ITimer *timer) = 0;
    };
} // namespace platform

#endif /* INC_PLATFORM_INTERFACES_ITIMER_HPP_ */

