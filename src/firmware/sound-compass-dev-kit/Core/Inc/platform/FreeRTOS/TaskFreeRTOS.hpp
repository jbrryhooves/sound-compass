/*
 * TaskFreeRTOS.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_FREERTOS_TASKFREERTOS_HPP_
#define INC_PLATFORM_FREERTOS_TASKFREERTOS_HPP_
//-------------------------------------------------------------------
// Module       : TaskFreeRTOS.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
extern "C"
{

#include "cmsis_os.h"
}

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::FreeRTOS
{
    class TaskFreeRTOS: public platform::ITask
    {
    public:
        virtual void taskMain(void) = 0;
    };

    class TaskFreeRTOSFactory: public platform::ITaskFactory
    {
    public:
        bool initialise(void);

        // ITaskFactory
        TaskHandle createTask(const char *taskName, unsigned int stackSize, int priority, unsigned int core, ITask *task);
        TaskHandle createTaskStatic(const char *taskName, void *taskControlBlock, unsigned int controlBlockSize, void *stackMemory, unsigned int stackSize, int priority,
                ITask *task);

        bool killTask(TaskHandle taskHandle);
    };
}

#endif /* INC_PLATFORM_FREERTOS_TASKFREERTOS_HPP_ */

