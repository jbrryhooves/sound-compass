/*
 * executiveStateMahcine.hpp
 *
 *  Created on: Oct 24, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_STATEMACHINE_EXECUTIVESTATEMACHINE_HPP_
#define INC_EXECUTIVE_STATEMACHINE_EXECUTIVESTATEMACHINE_HPP_
//-------------------------------------------------------------------
// Module       : executiveStateMahcine.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "etl/queue.h"
#include "etl/fsm.h"
#include "etl/message_packet.h"

#include "executive/executiveMain.hpp"
#include "executive/stateMachine/states/idleState.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
const etl::message_router_id_t EXECUTIVE_MESSAGE_ROUTER_ID = 0;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
namespace executive
{
    class ExecutiveStateMachine: public etl::fsm
    {
    public:
        ExecutiveStateMachine(executive::executiveMain * executive,  platform::IDiag* diag) :
                fsm(EXECUTIVE_MESSAGE_ROUTER_ID)
        {
            _executive = executive;
            _diag = diag;
        }


        enum class Messages: uint8_t
        {
            DebugHeartBeatMessage, DoWork, StopWork
        };
        enum class States: uint8_t
        {
            Idle, DoingWork, NUMBER_OF_STATES
        };

        void PrintDebugMessage(const char* message)
        {
            _diag->debug("StateMachine", message);
        }

        void PrintDebugMessage1()
        {
            _diag->debug("StateMachine", "message1");
        }
        void PrintDebugMessage2(int workAmount)
        {
            _diag->debug("StateMachine", "message2: %d", workAmount);
        }

        void LogUnknownEvent(etl::imessage &msg)
        {
            _diag->debug("StateMachine", "unknown message");
        }

        void SetStates()
        {
            this->set_states(stateList, etl::size(stateList));
        }

    private:
        executive::executiveMain * _executive;
        platform::IDiag* _diag;

        Idle _idleState;
        DoingWork _doingWorkState;

        // The states must be in state id order.
        etl::ifsm_state* stateList[executive::ExecutiveStateMachine::States::NUMBER_OF_STATES] ={ &_idleState, &_doingWorkState };

    };
}




#endif /* SRC_EXECUTIVE_STATEMACHINE_EXECUTIVESTATEMACHINE_HPP_ */

