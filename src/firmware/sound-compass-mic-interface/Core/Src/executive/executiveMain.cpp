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

    _messageQueue = _hardware->messageQueueFactory->createMessageQueueStatic("execMsgQueue", _messageQueueBuffer, MESSAGE_QUEUE_SIZE, sizeof(messages::Message));
    if (_messageQueue == nullptr)
    {
        _hardware->diag->error(TAG, "Could not create message queue");
        return false;
    }

//    _rawMicDataMessageQueue = _hardware->messageQueueFactory->createMessageQueueStatic("micDataMsgQueue", _micDataQueueBuffer,
//            executive::micInterface::MIC_BUFFER_MESSAGE_QUEUE_SIZE,
//            sizeof(MicArrayRawDataMessage*));

//    if (_rawMicDataMessageQueue == nullptr)
//    {
//        _hardware->diag->error(TAG, "Could not create mic data message queue");
//        return false;
//    }

    _debugTimerData.timerID = TimerID::DEBUG_LED;
    _debugTimer = _hardware->timerFactory->createTimerStatic("debug", &_debugTimerControlBlock, sizeof(_debugTimerControlBlock), true, this, &_debugTimerData);
    if (_debugTimer == nullptr)
    {
        _hardware->diag->error(TAG, "Could not create debug timer");
        return false;
    }

    _tcpServer.initialise(_hardware, this, _hardware->taskFactory);

    _micInterface.initialise(_hardware, this, _messageQueue);

    return true;
}


void executive::executiveMain::onControlCommandReceived(void)
{
//    DIAG_LINE_VERBOSE(_hardware->diag, TAG, "[audio frame processed]", "");

//    Message m;
//    m.type = MessageType::AUDIO_FRAME_PROCESSED;
//    _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);

}

//-------------------------------------------------------------------
// Public - micInterface
//-------------------------------------------------------------------


void executive::executiveMain::onError(executive::micInterface::MicInterfaceError error)
{

}

//-------------------------------------------------------------------
// Public - micInterface
//-------------------------------------------------------------------

void executive::executiveMain::onServerError(executive::tcpServer::TCPError error)
{
//    Message m;
//    m.type = MessageType::AUDIO_FRAME_METRICS;
//    m.data.metrics = *metrics;
//    _messageQueue->send(&m, MESSAGE_PRIORITY_NORMAL);

}

//-------------------------------------------------------------------
// Public - ITimerListener
//-------------------------------------------------------------------

void executive::executiveMain::onTimer(void *userData)
{
    TimerData *timerData = (TimerData*) userData;

    messages::Message m;
    switch (timerData->timerID)
    {
        case TimerID::DEBUG_LED:
//            DIAG_LINE_VERBOSE(_hardware->diag, TAG, "[DEBUG_LED]\n", "");

            m.type = messages::MessageType::DEBUG_LED;
            _messageQueue->send(&m);

            break;

    }
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;
    messages::Message _message;

    _hardware->diag->info(TAG, "Running...\n");

    _debugTimer->start(500);
    _tcpServer.start();
    _micInterface.start();

    while (1)
    {
        if (_messageQueue->receive(&_message, nullptr, 1000))
        {
            switch (_message.type)
            {
                case messages::MessageType::DEBUG_LED:
                    _hardware->LED_debugGreen->toggleLED();
//        _hardware->diag->info(TAG,  "Toggle at %d\n", __LINE__ );
//        DIAG_LINE_ERROR(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//                    DIAG_LINE_INFO(_hardware->diag, TAG, "Toggle %d\n", _count++);
//        DIAG_LINE_WARN(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_DEBUG(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_VERBOSE(_hardware->diag, TAG,  "Toggle %d\n", _count++);
                    break;
                case messages::MessageType::AUDIO_FRAME_PROCESSED:
                    _hardware->LED_debugOrange->toggleLED();
                    break;
                case messages::MessageType::AUDIO_FRAME_METRICS:

                    break;
                case messages::MessageType::AUDIO_FRAME_RAW:
                    _hardware->LED_debugOrange->toggleLED();
                    int8_t internalError;
                    if (executive::tcpServer::TCPError _err = _tcpServer.sendBytes((uint8_t*) (_message.data.micRawData->micData), sizeof(_message.data.micRawData->micData),
                            &internalError); _err
                            != executive::tcpServer::TCPError::Ok)
                    {
                        _hardware->diag->error(TAG, "Failed to send TCP audio frame: %d (LwIP Err: %d)\n", _err, internalError);

                    }

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

