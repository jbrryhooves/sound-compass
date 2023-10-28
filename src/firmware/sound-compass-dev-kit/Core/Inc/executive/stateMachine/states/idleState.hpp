/*
 * idleState.hpp
 *
 *  Created on: Oct 23, 2023
 *      Author: Josh
 */

#ifndef SRC_EXECUTIVE_STATEMACHINE_MESSAGES_IDLESTATE_HPP_
#define SRC_EXECUTIVE_STATEMACHINE_MESSAGES_IDLESTATE_HPP_
//-------------------------------------------------------------------
// Module       : idleState.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "executive/executiveMain.hpp"
#include "executive/stateMachine/executiveStateMachine.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{
    class HeartBeatMessage: public etl::message<ExecutiveStateMachine::Messages::DebugHeartBeatMessage>
    {
    };

    class DoWorkMessage: public etl::message<ExecutiveStateMachine::Messages::DoWork>
    {
    public:
        DoWorkMessage(int workAmount, bool continueWorking):workamount(workAmount), continueWorking(continueWorking)
        {

        }
        bool continueWorking;
        int workamount;
    };


    //***********************************
    // The idle state.
    // Accepts Start events.
    //***********************************
    class Idle: public etl::fsm_state<executive::ExecutiveStateMachine, Idle, ExecutiveStateMachine::States::Idle,
    ExecutiveStateMachine::Messages::DebugHeartBeatMessage,
    ExecutiveStateMachine::Messages::DoWork>
    {
    public:

        etl::fsm_state_id_t on_enter_state()  {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage("Entering Idle state");
            return STATE_ID;
        }
        etl::fsm_state_id_t on_exit_state()  {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage("Exiting  Idle state");
            return STATE_ID;
        }

        etl::fsm_state_id_t on_event(const HeartBeatMessage &event)
        {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage1();
            return ExecutiveStateMachine::States::Idle;
        }
        etl::fsm_state_id_t on_event(const DoWorkMessage &event)
        {
            return ExecutiveStateMachine::States::DoingWork;
        }

        etl::fsm_state_id_t on_event_unknown(const etl::imessage &event)
        {
            get_fsm_context().LogUnknownEvent(event);
            return STATE_ID;
        }
    };

    class DoingWork : public etl::fsm_state<executive::ExecutiveStateMachine, DoingWork, ExecutiveStateMachine::States::DoingWork,
    ExecutiveStateMachine::Messages::DebugHeartBeatMessage,
    ExecutiveStateMachine::Messages::DoWork>
    {
    public:

        etl::fsm_state_id_t on_enter_state()  {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage("Entering Work state");
            return STATE_ID;
        }
        etl::fsm_state_id_t on_exit_state()  {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage("Exiting  Work state");
            return STATE_ID;
        }

        etl::fsm_state_id_t on_event(const HeartBeatMessage &event)
        {
            ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage1();
            return ExecutiveStateMachine::States::Idle;
        }
        etl::fsm_state_id_t on_event(const DoWorkMessage &event)
        {
            if(event.continueWorking)
            {
                ((executive::ExecutiveStateMachine)get_fsm_context()).PrintDebugMessage2(event.workamount);
                return STATE_ID;
            }
            else
                return ExecutiveStateMachine::States::Idle;
        }

        etl::fsm_state_id_t on_event_unknown(const etl::imessage &event)
        {
            get_fsm_context().LogUnknownEvent(event);
            return STATE_ID;
        }
    };

}

#endif /* SRC_EXECUTIVE_STATEMACHINE_MESSAGES_IDLESTATE_HPP_ */

