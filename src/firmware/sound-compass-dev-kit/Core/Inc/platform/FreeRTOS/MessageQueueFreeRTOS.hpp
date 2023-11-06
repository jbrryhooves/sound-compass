/*
 * MessageQueueFreeRTOS.hpp
 *
 *  Created on: Oct 29, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_FREERTOS_MESSAGEQUEUEFREERTOS_HPP_
#define INC_PLATFORM_FREERTOS_MESSAGEQUEUEFREERTOS_HPP_
//-------------------------------------------------------------------
// Module       : MessageQueueFreeRTOS.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include "cmsis_os.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/IMessageQueue.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
typedef osMessageQueueId_t QueueHandle_t;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------


  namespace platform::FreeRTOS
{
    class MessageQueueFreeRTOS : public platform::IMessageQueue
    {
    public:
        MessageQueueFreeRTOS();
        virtual ~MessageQueueFreeRTOS();

        bool initialise(unsigned int numberOfItems, unsigned int itemSize);
        bool initialise(const char* name, void * controlBlock, unsigned int controlBlockSize, void * queueBuffer, unsigned int numberOfItems, unsigned int itemSize);

        // IMessageQueue
        bool send(void* message, unsigned int priority = 0, unsigned int timeoutMs = 0);
        bool sendFromISR(void* message, unsigned int priority = 0);
        bool receive(void* message, unsigned int* priority = nullptr, unsigned int timeoutMs = DEFAULT_MESSAGE_QUEUE_TIMEOUT_MS);
        bool getNumberOfQueuedItems(unsigned int* count);

    private:
        QueueHandle_t _messageQueue;
    };

    class MessageQueueFreeRTOSFactory : public platform::IMessageQueueFactory
    {
    public:
        bool initialise(void);

        // IMessageQueueFactory
        platform::IMessageQueue* createMessageQueue(const char* name, unsigned int numberOfItems, unsigned int itemSize);
        platform::IMessageQueue* createMessageQueueStatic(const char* name, void * queueBuffer, unsigned int numberOfItems, unsigned int itemSize);
    };
}




#endif /* INC_PLATFORM_FREERTOS_MESSAGEQUEUEFREERTOS_HPP_ */

