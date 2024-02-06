/*
 * executive.hpp
 *
 *  Created on: Sep 18, 2023
 *      Author: Josh
 */

#ifndef __EXECUTIVE_HPP_
#define __EXECUTIVE_HPP_

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

#include <stdbool.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

#include "platform/interfaces/IExecutive.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/IMessageQueue.hpp"
#include "platform/interfaces/ITimer.hpp"

#include "executive/tcpServer.hpp"


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class executiveMain: public platform::IExecutive, public executive::tcpServer::ITCPServerListener, public platform::ITimer::ITimerListener
    {
    public:

        // IExecutive
        bool initialise(board::IBoardHardware *hw);
        bool run(void);

        // ITimerListener
        void onTimer(void *userData);

        // ITCPServerListener
        void onControlCommandReceived(void);
        void onServerError(executive::tcpServer::TCPError error);

    private:
        // Injected hardware
        board::IBoardHardware *_hardware;

        // Message queue
        static const unsigned int MESSAGE_QUEUE_SIZE = 10;
        const unsigned int MESSAGE_PRIORITY_QUIT = 11;
        const unsigned int MESSAGE_PRIORITY_NORMAL = 0;
        typedef enum
        {
            QUIT, STATE_TRANSIT, STATE_TIMER,
            DEBUG_LED,
            AUDIO_FRAME_PROCESSED,
            AUDIO_FRAME_METRICS
        } MessageType;

        typedef struct
        {
            MessageType type;
//            union
//            {
//                executive::audioProcessor::ProcessingMetrics_t metrics;
//            } data;
        } Message;
        Message _messageQueueBuffer[MESSAGE_QUEUE_SIZE];

        platform::IMessageQueue *_messageQueue;

        // Timers
        enum class TimerID
        {
            DEBUG_LED,
        };
        typedef struct
        {
            TimerID timerID;
        } TimerData;
        platform::ITimer *_debugTimer;
        TimerData _debugTimerData;
        platform::ITimer *_stateTimer;
        TimerData _stateTimerData;

        executive::tcpServer _tcpServer;

    };

} /* namespace executive */

#endif /* __EXECUTIVE_HPP_ */
