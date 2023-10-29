
//-------------------------------------------------------------------
// Module       : audioProcessor.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
extern "C"
{

#include "cmsis_os.h"
}

#include "stdio.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

#include "executive/audioProcessor.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char* TAG = "audioProc";


osThreadId_t audioProcessorTaskHandle;
uint32_t audioProcessorTaskBuffer[ 1024 ];
StaticTask_t audioProcessorTaskControlBlock;
const osThreadAttr_t audioProcessorTask_attributes = {
  .name = "audioProcessorTask",
  .cb_mem = &audioProcessorTaskControlBlock,
  .cb_size = sizeof(audioProcessorTaskControlBlock),
  .stack_mem = &audioProcessorTaskBuffer[0],
  .stack_size = sizeof(audioProcessorTaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------


bool executive::audioProcessor::initialise(board::IBoardHardware *hardware, platform::ITaskFactory* taskFactory)
{
    _hardware  = hardware;
    _taskFactory = taskFactory;
    return true;
}

bool executive::audioProcessor::start()
{
    _hardware->diag->info(TAG, "Starting..\n");
    _taskHandle = _taskFactory->createTaskStatic(
            "audioProcessorTask",
            &audioProcessorTaskControlBlock,
            sizeof(audioProcessorTaskControlBlock),
            &audioProcessorTaskBuffer[0],
            sizeof(audioProcessorTaskBuffer),
            osPriorityAboveNormal,
            this // ITask
            );

    if (_taskHandle == nullptr)
    {
        _hardware->diag->error(TAG, "Can not create audioProcessorTask task");
        return false;
    }
    return true;
}

// ITask
void executive::audioProcessor::taskMain(void)
{
    while(1){
//        printf("audio processor\n");

        _hardware->LED_debugOrange->toggleLED();
        osDelay(70);

    }
}

bool executive::audioProcessor::killEnable(bool enable)
{

}
