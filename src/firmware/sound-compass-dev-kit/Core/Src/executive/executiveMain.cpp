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
    if (_debugTimer == nullptr)
    {
        _hardware->diag->error(TAG, "Could not create debug timer");
        return false;
    }

    _audioProcessor.initialise(_hardware, this, _hardware->taskFactory);

    return true;
}

void executive::executiveMain::onAudioFrameProcessed(void)
{
//    DIAG_LINE_VERBOSE(_hardware->diag, TAG, "[audio frame processed]", "");

    Message m;
    m.type = MessageType::AUDIO_FRAME_PROCESSED;
    _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);

}

void executive::executiveMain::onAudioFrameMetrics(const executive::audioProcessor::ProcessingMetrics_t *metrics)
{
    Message m;
    m.type = MessageType::AUDIO_FRAME_METRICS;
    m.data.metrics = *metrics;
    _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);

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

            m.type = MessageType::DEBUG_LED;
            _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);
            break;

    }
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;
    Message _message;

    _hardware->diag->info(TAG, "Running...\n");

    _debugTimer->start(500);
    _audioProcessor.start();

    while (1)
    {
        if (_messageQueue->receive(&_message, nullptr, 1000))
        {
            switch (_message.type)
            {
                case MessageType::DEBUG_LED:
                    _hardware->LED_debugGreen->toggleLED();
//        _hardware->diag->info(TAG,  "Toggle at %d\n", __LINE__ );
//        DIAG_LINE_ERROR(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//                    DIAG_LINE_INFO(_hardware->diag, TAG, "Toggle %d\n", _count++);
//        DIAG_LINE_WARN(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_DEBUG(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_VERBOSE(_hardware->diag, TAG,  "Toggle %d\n", _count++);
                    break;
                case MessageType::AUDIO_FRAME_PROCESSED:
                    _hardware->LED_debugOrange->toggleLED();
                    break;
                case MessageType::AUDIO_FRAME_METRICS:
                    _hardware->diag->info(TAG, "Frame metrics: \n filter count: %d\n full time:%d\n", _message.data.metrics.filterCount,
                            _message.data.metrics.fullProcessingTime_ms);
                    break;
                default:
                    _hardware->diag->error(TAG, "Unhandled message type: %d\n", _message.type);
                    break;
            }
        }

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

