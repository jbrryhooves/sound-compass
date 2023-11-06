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

        class IAudioProcessorListener
        {
        public:
            virtual void onAudioFrameProcessed(void) = 0;
        };

        bool initialise(board::IBoardHardware *hardware, IAudioProcessorListener *audioProcessedListener, platform::ITaskFactory *taskFactory);
        bool start();

        // ITask
        void taskMain(void);
        bool killEnable(bool enable);

    private:

//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;
        platform::ITaskFactory *_taskFactory;
        platform::ITaskFactory::TaskHandle _taskHandle;
        IAudioProcessorListener *_audioProcessedListener;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_AUDIOPROCESSOR_HPP_ */

