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
#include "executive/micInterface.hpp"
#include "executive/messages.hpp"


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class executiveMain: public platform::IExecutive,
            public executive::tcpServer::ITCPServerListener,
            public platform::ITimer::ITimerListener,
            public executive::micInterface::IMicInterfaceListener
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

        // IMicInterfaceListener
        void onError(executive::micInterface::MicInterfaceError error);

    private:
        // Injected hardware
        board::IBoardHardware *_hardware;

        // Message queue
        static const unsigned int MESSAGE_QUEUE_SIZE = 4;
        const unsigned int MESSAGE_PRIORITY_QUIT = 11;
        const unsigned int MESSAGE_PRIORITY_NORMAL = 0;

        // the message queue will hold pointers to the raw data to prevent unnecessary copying
//        MicArrayRawDataMessage *_micDataQueueBuffer[executive::micInterface::MIC_BUFFER_MESSAGE_QUEUE_SIZE];
        platform::IMessageQueue *_rawMicDataMessageQueue;

        executive::messages::Message _messageQueueBuffer[MESSAGE_QUEUE_SIZE];
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
        executive::micInterface _micInterface;

    };

} /* namespace executive */

#endif /* __EXECUTIVE_HPP_ */
