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

//#include "etl/message_packet.h"

#include "platform/interfaces/IExecutive.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/IMessageQueue.hpp"
#include "platform/interfaces/ITimer.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class executiveMain: public platform::IExecutive
    {
    public:

        // IExecutive
        bool initialise(board::IBoardHardware *hw);
        bool run(void);

    private:
        enum class MessageType
        {
            DebugHeartBeatMessage, Message2, ButtonPress
        };

        typedef struct
        {
            MessageType type;
        } Message;

        // Injected hardware
        board::IBoardHardware *_hardware;

        platform::IMessageQueue *_messageQueue;
        platform::ITimer *_debugTimer;

    };

} /* namespace executive */

#endif /* __EXECUTIVE_HPP_ */
