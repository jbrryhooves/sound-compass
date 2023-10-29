/*
 * ITask.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_ITASK_HPP_
#define INC_PLATFORM_INTERFACES_ITASK_HPP_
//-------------------------------------------------------------------
// Module       : ITask.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class ITask
    {
    public:
        virtual ~ITask()
        {
        }

        /**
         * @brief The task mainline
         *
         */
        virtual void taskMain(void) = 0;

        /**
         * @brief Set kill enable for task
         * Allows holding off killing if busy.
         * e.g for Linux: https://man7.org/linux/man-pages/man3/pthread_cancel.3.html
         *
         * @param enable
         * @return true success
         * @return false
         */
        virtual bool killEnable(bool enable) = 0;

        /**
         * @brief This is the static entry for the task
         * This is static so C task implementations can
         * use taskHook as the entry point.
         *
         * @param parameters
         */
        static void taskHook(void* parameters)
        {
            ITask* pThis = (ITask*)parameters;
            pThis->taskMain();
        }
    };

    class ITaskFactory
    {
    public:
        typedef void* TaskHandle;

        /**
         * @brief Create a Task
         *
         * @param taskName
         * @param stackSize
         * @param priority
         * @param core
         * @param task
         * @return TaskHandle
         */
        virtual TaskHandle createTask(const char* taskName, unsigned int stackSize, int priority, unsigned int core, ITask* task) = 0;


        virtual TaskHandle createTaskStatic(const char *taskName, void *taskControlBlock, unsigned int controlBlockSize, void *stackMemory, unsigned int stackSize, int priority, ITask *task) = 0;

        /**
         * @brief Kill running task
         *
         * @param taskHandle
         * @return true
         * @return false
         */
        virtual bool killTask(TaskHandle taskHandle) = 0;
    };
} // namespace platform




#endif /* INC_PLATFORM_INTERFACES_ITASK_HPP_ */

