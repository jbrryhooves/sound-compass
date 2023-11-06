/*
 * TimerFreeRTOS.hpp
 *
 *  Created on: Nov 5, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_FREERTOS_TIMERFREERTOS_HPP_
#define INC_PLATFORM_FREERTOS_TIMERFREERTOS_HPP_
//-------------------------------------------------------------------
// Module       : TimerFreeRTOS.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
extern "C"
{
#include "cmsis_os.h"

#include "timers.h" // freertos/timers.h
}
//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITimer.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::FreeRTOS
{
    class TimerFreeRTOS: public platform::ITimer
    {
    public:
        virtual ~TimerFreeRTOS()
        {
        }
        ;

        bool initialise(const char *name, TimerHandle_t handle, platform::ITimer::ITimerListener *listener, void *userData);

        // ITimer
        bool stop(void);
        bool stopFromISR(void);
        bool start(unsigned int ms);
        bool startFromISR(void);
        bool reset(void);
        bool resetFromISR(void);
        bool setPeriod(unsigned int ms);

        // INamed
        void setName(const char *name);
        const char* getName(void);

        void timerHandler(void);

    private:
        const char *_name;
        TimerHandle_t _handle;
        platform::ITimer::ITimerListener *_listener;
        void *_userData;
    };

    class TimerFreeRTOSFactory: public platform::ITimerFactory
    {
    public:
        bool initialise(void);
        platform::ITimer* createTimer(const char *name, unsigned int ms, bool autoRestart, platform::ITimer::ITimerListener *listener, void *userData);
        platform::ITimer* createTimerStatic(const char *timerName, void *timerControlBlock, unsigned int controlBlockSize, bool periodic,
                platform::ITimer::ITimerListener *listener, void *userData);
        bool destroyTimer(platform::ITimer *timer);
    };
}

#endif /* INC_PLATFORM_FREERTOS_TIMERFREERTOS_HPP_ */

