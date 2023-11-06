/*
 * IMessageQueue.hpp
 *
 *  Created on: Oct 16, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IMESSAGEQUEUE_HPP_
#define INC_PLATFORM_INTERFACES_IMESSAGEQUEUE_HPP_
//-------------------------------------------------------------------
// Module       : IMessageQueue.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <limits.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
#define DEFAULT_MESSAGE_QUEUE_TIMEOUT_MS UINT_MAX

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class IMessageQueue
    {
    public:
        virtual ~IMessageQueue()
        {
        }

        /**
         * @brief Send message
         *
         * @param message
         * @param priority
         * @param timeoutMs
         * @return true
         * @return false
         */
        virtual bool send(void* message, unsigned int priority = 0, unsigned int timeoutMs = 0) = 0;

        /**
         * @brief Send message from ISR
         *
         * @param message
         * @param priority
         * @return true
         * @return false error
         */
        virtual bool sendFromISR(void* message, unsigned int priority = 0) = 0;

        /**
         * @brief Receive message
         *
         * @param message
         * @param priority
         * @param timeoutMs
         * @return true
         * @return false timeout
         */
        virtual bool
        receive(void* message, unsigned int* priority = nullptr, unsigned int timeoutMs = DEFAULT_MESSAGE_QUEUE_TIMEOUT_MS) = 0;

        /**
         * @brief Get the Number Of Queued Items
         *
         * @param count
         * @return true
         * @return false
         */
        virtual bool getNumberOfQueuedItems(unsigned int* count) = 0;
    };

    class IMessageQueueFactory
    {
    public:
        /**
         * @brief Create a Message Queue
         *
         * @param name
         * @param numberOfItems
         * @param itemSize
         * @return IMessageQueue*
         */
        virtual IMessageQueue* createMessageQueue(const char* name, unsigned int numberOfItems, unsigned int itemSize) = 0;
        virtual IMessageQueue* createMessageQueueStatic(const char* name, void * queueBuffer, unsigned int numberOfItems, unsigned int itemSize) = 0;
    };

}



#endif /* INC_PLATFORM_INTERFACES_IMESSAGEQUEUE_HPP_ */

