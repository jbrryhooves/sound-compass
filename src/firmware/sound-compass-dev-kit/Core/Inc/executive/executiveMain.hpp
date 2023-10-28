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

#include "executive/stateMachine/executiveStateMachine.hpp"

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

        ExecutiveStateMachine _stateMachine;
        // Injected hardware
        board::IBoardHardware *_hardware;

        platform::IMessageQueue *_messageQueue;
        platform::ITimer *_debugTimer;

    };

} /* namespace executive */

#endif /* __EXECUTIVE_HPP_ */
