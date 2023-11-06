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

#include "executive/audioProcessor.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class executiveMain: public platform::IExecutive, public executive::audioProcessor::IAudioProcessorListener, public platform::ITimer::ITimerListener
    {
    public:

        // IExecutive
        bool initialise(board::IBoardHardware *hw);
        bool run(void);

        // ITimerListener
        void onTimer(void *userData);

        // IAudioProcessorListener
        void onAudioFrameProcessed(void);

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

            AUDIO_FRAME_PROCESSED,
        } MessageType;

        typedef struct
        {
            MessageType type;
            union
            {
                char dtmfCode;
            } data;
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

        audioProcessor _audioProcessor;

    };

} /* namespace executive */

#endif /* __EXECUTIVE_HPP_ */
