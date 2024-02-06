//-------------------------------------------------------------------
// Module       : TimerFreeRTOS.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/FreeRTOS/TimerFreeRTOS.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
//static void TimerHandler(TimerHandle_t timer);

//-------------------------------------------------------------------
// C static
//-------------------------------------------------------------------
static void _TimerHandler(void *timerArg)
{
    platform::FreeRTOS::TimerFreeRTOS *pThis = (platform::FreeRTOS::TimerFreeRTOS*) timerArg;
    pThis->timerHandler();
}

static void _DynamicTimerHandler(TimerHandle_t timer)
{
    platform::FreeRTOS::TimerFreeRTOS *pThis = (platform::FreeRTOS::TimerFreeRTOS*) (pvTimerGetTimerID(timer));
    pThis->timerHandler();
}

void platform::FreeRTOS::TimerFreeRTOS::timerHandler(void)
{
    if (_listener != nullptr)
    {
        _listener->onTimer(_userData);
    }
}

//-------------------------------------------------------------------
// Publc - ITimer
//-------------------------------------------------------------------
bool platform::FreeRTOS::TimerFreeRTOS::stop(void)
{
    return xTimerStop(_handle, 0);
}

bool platform::FreeRTOS::TimerFreeRTOS::stopFromISR(void)
{
    return xTimerStopFromISR(_handle, 0);
}

bool platform::FreeRTOS::TimerFreeRTOS::start(unsigned int ms)
{
    uint32_t ticks = (TickType_t) (ms / portTICK_PERIOD_MS);
    return osTimerStart(_handle, ticks) == osOK;
}

bool platform::FreeRTOS::TimerFreeRTOS::startFromISR(void)
{
    return xTimerStartFromISR(_handle, 0);
}

bool platform::FreeRTOS::TimerFreeRTOS::reset(void)
{
    return xTimerReset(_handle, 0);
}

bool platform::FreeRTOS::TimerFreeRTOS::resetFromISR(void)
{
    return xTimerResetFromISR(_handle, 0);
}

bool platform::FreeRTOS::TimerFreeRTOS::setPeriod(unsigned int ms)
{
    return xTimerChangePeriod(_handle, ms / portTICK_PERIOD_MS, 100);
}

//-------------------------------------------------------------------
// Publc - INamed
//-------------------------------------------------------------------
void platform::FreeRTOS::TimerFreeRTOS::setName(const char *name)
{
    _name = name;
}

const char* platform::FreeRTOS::TimerFreeRTOS::getName(void)
{
    return _name;
}

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
bool platform::FreeRTOS::TimerFreeRTOS::initialise(const char *name, TimerHandle_t handle, platform::ITimer::ITimerListener *listener, void *userData)
{
    _name = name;
    _handle = handle;
    _listener = listener;
    _userData = userData;
    return true;
}

//-------------------------------------------------------------------
// Public - ITimerFactory
//-------------------------------------------------------------------
bool platform::FreeRTOS::TimerFreeRTOSFactory::initialise(void)
{
    return true;
}

platform::ITimer* platform::FreeRTOS::TimerFreeRTOSFactory::createTimer(const char *name, unsigned int ms, bool autoRestart, platform::ITimer::ITimerListener *listener,
        void *userData)
{
    TimerFreeRTOS *timer = new TimerFreeRTOS();

    TimerHandle_t timerHandle = xTimerCreate(name, (TickType_t) (ms / portTICK_PERIOD_MS), autoRestart, (void*) timer, _DynamicTimerHandler);

    if (!timer->initialise(name, timerHandle, listener, userData))
    {
        delete (TimerFreeRTOS*) timer;
        return nullptr;
    }

    return timer;
}

platform::ITimer* platform::FreeRTOS::TimerFreeRTOSFactory::createTimerStatic(const char *timerName, void *timerControlBlock, unsigned int controlBlockSize, bool periodic,
        platform::ITimer::ITimerListener *listener, void *userData)
{
    TimerFreeRTOS *timer = new TimerFreeRTOS();

    const osTimerAttr_t timerAttributes =
    { .name = timerName, .cb_mem = timerControlBlock, .cb_size = controlBlockSize, };

    TimerHandle_t timerHandle = (TimerHandle_t) osTimerNew((osTimerFunc_t) _TimerHandler, periodic ? osTimerPeriodic : osTimerOnce, (void*) timer, &timerAttributes);

    if (!timer->initialise(timerName, timerHandle, listener, userData))
    {
        delete (TimerFreeRTOS*) timer;
        return nullptr;
    }

    return timer;
}

bool platform::FreeRTOS::TimerFreeRTOSFactory::destroyTimer(platform::ITimer *timer)
{
    delete (TimerFreeRTOS*) timer;
    return true;
}

