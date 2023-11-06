//-------------------------------------------------------------------
// Module       : executiveMain.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

extern "C"
{
#include "cmsis_os.h"
}

#include "string.h"
#include <stdio.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
//#include "etl/queue.h"
//#include "etl/message_packet.h"

#include "executive/executiveMain.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/IDiag.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char *TAG = "Exec";

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
typedef StaticTimer_t osStaticTimerDef_t;
osStaticTimerDef_t _debugTimerControlBlock;

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool executive::executiveMain::initialise(board::IBoardHardware *hardware)
{
    _hardware = hardware;

    _messageQueue = _hardware->messageQueueFactory->createMessageQueueStatic("execMsgQueue", _messageQueueBuffer, MESSAGE_QUEUE_SIZE, sizeof(Message));
    if (_messageQueue == nullptr)
    {
        _hardware->diag->error(TAG, "Could not create message queue");
        return false;
    }

    _debugTimerData.timerID = TimerID::DEBUG_LED;
    _debugTimer = _hardware->timerFactory->createTimerStatic("debug", &_debugTimerControlBlock, sizeof(_debugTimerControlBlock), true, this, &_debugTimerData);

    _audioProcessor.initialise(_hardware, this, _hardware->taskFactory);

    return true;
}

void executive::executiveMain::onAudioFrameProcessed(void)
{
//    DIAG_LINE_VERBOSE(_hardware->diag, TAG, "[audio frame processed]", "");
    _hardware->LED_debugOrange->toggleLED();
}

//-------------------------------------------------------------------
// Public - ITimerListener
//-------------------------------------------------------------------

void executive::executiveMain::onTimer(void *userData)
{
    TimerData *timerData = (TimerData*) userData;

    Message m;
    switch (timerData->timerID)
    {
        case TimerID::DEBUG_LED:
//            DIAG_LINE_VERBOSE(_hardware->diag, TAG, "[DEBUG_LED]\n", "");
            _hardware->LED_debugGreen->toggleLED();
            m.type = MessageType::STATE_TIMER;
//            _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);
            break;

    }
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;

    _debugTimer->start(2000);
    _audioProcessor.start();

    while (1)
    {


//        _hardware->diag->info(TAG,  "Toggle at %d\n", __LINE__ );
//        DIAG_LINE_ERROR(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_INFO(_hardware->diag, TAG, "Toggle %d\n", _count++);
//        DIAG_LINE_WARN(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_DEBUG(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_VERBOSE(_hardware->diag, TAG,  "Toggle %d\n", _count++);

        osDelay(500);

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

