/*
 * audioProcessor.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_AUDIOPROCESSOR_HPP_
#define INC_EXECUTIVE_AUDIOPROCESSOR_HPP_
//-------------------------------------------------------------------
// Module       : audioProcessor.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"
#include "board/interfaces/IBoardHardware.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class audioProcessor: public platform::ITask
    {
    public:

        bool initialise(board::IBoardHardware *hardware, platform::ITaskFactory* taskFactory);
        bool start();

        // ITask
        void taskMain(void);
        bool killEnable(bool enable);

    private:

//        platform::IMessageQueue *_messageQueue;
//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;
        platform::ITaskFactory* _taskFactory;
        platform::ITaskFactory::TaskHandle _taskHandle;

    };

} /* namespace executive */



#endif /* INC_EXECUTIVE_AUDIOPROCESSOR_HPP_ */

