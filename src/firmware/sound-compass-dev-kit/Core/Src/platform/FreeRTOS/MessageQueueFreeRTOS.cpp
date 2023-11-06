//-------------------------------------------------------------------
// Module       : MessageQueueFreeRTOS.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdlib.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/FreeRTOS/MessageQueueFreeRTOS.hpp"

extern "C"
{
#include "cmsis_os.h"
}

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
bool platform::FreeRTOS::MessageQueueFreeRTOS::send(void *message, unsigned int priority, unsigned int timeoutMs)
{
    // priority unused
    return osMessageQueuePut(_messageQueue, message, priority, (TickType_t) (timeoutMs / portTICK_RATE_MS)) == osOK;
}

bool platform::FreeRTOS::MessageQueueFreeRTOS::sendFromISR(void *message, unsigned int priority)
{
    // priority unused
    return osMessageQueuePut(_messageQueue, message, priority, 0) == osOK;
    return false;
}

bool platform::FreeRTOS::MessageQueueFreeRTOS::receive(void *message, unsigned int *priority, unsigned int timeoutMs)
{
    // priority unused
    return osMessageQueueGet(_messageQueue, message, (uint8_t*) priority, (TickType_t) (timeoutMs / portTICK_RATE_MS)) == osOK;
}

bool platform::FreeRTOS::MessageQueueFreeRTOS::getNumberOfQueuedItems(unsigned int *count)
{
    *count = (unsigned int) osMessageQueueGetCount(_messageQueue);
    return true;
}

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
platform::FreeRTOS::MessageQueueFreeRTOS::MessageQueueFreeRTOS()
{
    _messageQueue = nullptr;
}

platform::FreeRTOS::MessageQueueFreeRTOS::~MessageQueueFreeRTOS()
{
    if (_messageQueue != nullptr)
    {
        osMessageQueueDelete(_messageQueue);
    }
}

bool platform::FreeRTOS::MessageQueueFreeRTOS::initialise(unsigned int numberOfItems, unsigned int itemSize)
{
//    _messageQueue = xQueueCreate(numberOfItems, itemSize);
//    if (_messageQueue == nullptr)
//        return false;
    return false;
}

/// Initialise a statically allocated queue
bool platform::FreeRTOS::MessageQueueFreeRTOS::initialise(const char *name, void *controlBlock, unsigned int controlBlockSize, void *queueBuffer, unsigned int numberOfItems,
        unsigned int itemSize)
{
    const osMessageQueueAttr_t _queue_attributes =
    { .name = name, .cb_mem = controlBlock, .cb_size = controlBlockSize, .mq_mem = queueBuffer, .mq_size = numberOfItems * itemSize };

    _messageQueue = osMessageQueueNew(numberOfItems, itemSize, &_queue_attributes);
    if (_messageQueue == nullptr)
        return false;
    return true;
}

//-------------------------------------------------------------------
// Public - IMessageQueueFactory
//-------------------------------------------------------------------
bool platform::FreeRTOS::MessageQueueFreeRTOSFactory::initialise(void)
{
    return true;
}

platform::IMessageQueue* platform::FreeRTOS::MessageQueueFreeRTOSFactory::createMessageQueue(const char *name, unsigned int numberOfItems, unsigned int itemSize)
{
    MessageQueueFreeRTOS *messageQueue = new MessageQueueFreeRTOS();
    if (messageQueue == nullptr)
        return nullptr;

    if (!messageQueue->initialise(numberOfItems, itemSize))
    {
        delete messageQueue;
        return nullptr;
    }
    return messageQueue;
}

platform::IMessageQueue* platform::FreeRTOS::MessageQueueFreeRTOSFactory::createMessageQueueStatic(const char *name, void *queueBuffer, unsigned int numberOfItems,
        unsigned int itemSize)
{
    MessageQueueFreeRTOS *messageQueue = new MessageQueueFreeRTOS();
    if (messageQueue == nullptr)
        return nullptr;

    StaticQueue_t *_controlBlock = (StaticQueue_t*) calloc(1, sizeof(StaticQueue_t));
    if (_controlBlock == nullptr)
    {
        delete messageQueue;
        return nullptr;
    }

    if (!messageQueue->initialise(name, _controlBlock, sizeof(StaticQueue_t), queueBuffer, numberOfItems, itemSize))
    {
        delete messageQueue;
        delete _controlBlock;

        return nullptr;
    }
    return messageQueue;
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

