//-------------------------------------------------------------------
// Module       : TaskFreeRTOS.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/FreeRTOS/TaskFreeRTOS.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public - TaskFreeRTOSFactory
//-------------------------------------------------------------------
bool platform::FreeRTOS::TaskFreeRTOSFactory::initialise(void)
{
    return true;
}

platform::ITaskFactory::TaskHandle platform::FreeRTOS::TaskFreeRTOSFactory::createTask(const char* taskName, unsigned int stackSize, int priority, unsigned int core, ITask* task)
{
//    if (stackSize < configMINIMAL_STACK_SIZE)
//    {
//        stackSize = configMINIMAL_STACK_SIZE;
//    }
//    osThreadId_t taskHandle = nullptr;
////    xTaskCreatePinnedToCore(platform::ITask::taskHook, taskName, stackSize, (void*)task, (UBaseType_t)priority, &taskHandle, (BaseType_t)core);
//
//    taskHandle = osThreadNew((osThreadFunc_t) task->taskHook, NULL, defaultTask_attributes);
//    return taskHandle;
    return nullptr;
}

platform::ITaskFactory::TaskHandle platform::FreeRTOS::TaskFreeRTOSFactory::createTaskStatic(const char *taskName, void *taskControlBlock, unsigned int controlBlockSize, void *stackMemory, unsigned int stackSize, int priority, ITask *task)
{
    if (stackSize < configMINIMAL_STACK_SIZE)
    {
        stackSize = configMINIMAL_STACK_SIZE;
    }
    const osThreadAttr_t task_attributes =
    { .name = taskName, .cb_mem = taskControlBlock, .cb_size = controlBlockSize, .stack_mem = stackMemory,
            .stack_size = stackSize, .priority = (osPriority_t) priority };

    osThreadId_t taskHandle = nullptr;
//    xTaskCreatePinnedToCore(platform::ITask::taskHook, taskName, stackSize, (void*)task, (UBaseType_t)priority, &taskHandle, (BaseType_t)core);

    taskHandle = osThreadNew((osThreadFunc_t) task->taskHook, task, &task_attributes);
    return taskHandle;
}

bool platform::FreeRTOS::TaskFreeRTOSFactory::killTask(platform::ITaskFactory::TaskHandle taskHandle)
{
//    if (taskHandle != nullptr)
//    {
//        vTaskDelete(taskHandle);
//    }
    return false;
}
