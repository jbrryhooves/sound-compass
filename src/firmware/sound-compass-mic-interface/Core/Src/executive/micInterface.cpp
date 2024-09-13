//-------------------------------------------------------------------
// Module       : micInnerface.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
extern "C"
{
#include "cmsis_os.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "string.h"
#include "queue.h"
}
#include "stdio.h"


//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "executive/constants.hpp"
#include "executive/micInterface.hpp"
#include "util/assert.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char *TAG = "micInterface";


//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

// @formatter:off
// @formatter:on
// =============================================================


bool executive::micInterface::initialise(board::IBoardHardware *hardware, IMicInterfaceListener *micInterfaceListener, platform::IMessageQueue *rawMicDataQueue)
{
    _hardware = hardware;
    _micInterfaceListener = micInterfaceListener;
    _execMessageQueue = rawMicDataQueue;

    _rawMicDataMessageQueue = _hardware->messageQueueFactory->createMessageQueueStatic("micDataMsgQueue", _micDataQueueBuffer,
            MIC_BUFFER_MESSAGE_QUEUE_SIZE,
            sizeof(messages::MicArrayRawDataMessage*));

    if (_rawMicDataMessageQueue == nullptr)
    {
        _hardware->diag->error(TAG, "Could not create mic data message queue");
        return false;
    }

    executive::ADC_AD7768::AD7768Config
    config =
    {
        .spiPort = _hardware->SPI,
        .sai = _hardware->SAI,
        .sampleRateTimer = _hardware->HardwareTimer1,
        .chipSelectPinMaster =
            _hardware->GPIO_SPI_nCS1, .chipSelectPinSlave = _hardware->GPIO_SPI_nCS2, .DRDYPin = _hardware->GPIO_DRDY, .startPin = _hardware->GPIO_START
    };

    if (!_ad7768.initialise(_hardware, config, this))
    {
        _hardware->diag->info(TAG, "Failed to initialise AD7768\n");
        return false;
    }

    if (!_ad7768.configure())
    {
        _hardware->diag->info(TAG, "Failed to configure AD7768\n");
        return false;
    }

    _hardware->diag->info(TAG, "Mic Interface init\n");
    return true;
}


bool executive::micInterface::start()
{
    _hardware->diag->info(TAG, "Starting..\n");

//    memset(pRxData, 0, BUFFER_ALIGNED_SIZE);
//
//    for (uint32_t i = 0; i < executive::messages::BUFFER_LENGTH_SAMPLES; i++)
//    {
//        for (uint32_t m = 0; m < NUMBER_OF_MICS; m++)
//        {
//            uint32_t index = i * NUMBER_OF_MICS * sizeof(uint32_t) + m * sizeof(uint32_t);
//            uint32_t val = i + m;
//            pTxData[index] = (val >> 0) & 0xFF;
//            pTxData[index + 1] = (val >> 8) & 0xFF;
//            pTxData[index + 2] = (val >> 16) & 0xFF;
//            pTxData[index + 3] = (val >> 24) & 0xFF;
//        }
//    }
//
//    if (!_spiPort->transmitReceiveDMA(pTxData, pRxData, BUFF_LENGTH))
//    {
//        _hardware->diag->error(TAG, "Failed to transmit");
//    }

    _ad7768.start();
    return true;
}

bool executive::micInterface::popMicDataMessage(messages::MicArrayRawDataMessage **message)
{
    messages::MicArrayRawDataMessage *_m;
    if (_rawMicDataMessageQueue->receive(&_m))
    {
        *message = _m;
        return true;
    }
    return false;

}


void executive::micInterface::onSample(ADC_AD7768::ADCSimultaneousSample &sample)
{
    _micDataQueueBuffer[_currentBufferIndex].header.messageLength = sizeof(messages::MicArrayRawDataMessage);
    _micDataQueueBuffer[_currentBufferIndex].header.type = messages::ExternalMessageType::AUDIO_FRAME_RAW;

    static uint16_t sampleIndex = 0;
    static bool _localBufferOverrun = false;

    if (sampleIndex == 0)
    {
        _micDataQueueBuffer[_currentBufferIndex].timeStamp = _bufferSequenceNumber;
        _micDataQueueBuffer[_currentBufferIndex].sequenceNumber = _bufferSequenceNumber++;
    }

    for (auto i = 0; i < NUMBER_OF_MICS; i++)
    {
        _micDataQueueBuffer[_currentBufferIndex].micData[i][sampleIndex] = sample.samples[i];
    }
    sampleIndex = (sampleIndex + 1) % executive::messages::BUFFER_LENGTH_SAMPLES;

    if (sampleIndex == 0)
    {
        // post a message to the local queue, which is size-bound
        messages::MicArrayRawDataMessage *_micBuffMessage;
        _micBuffMessage = &_micDataQueueBuffer[_currentBufferIndex];
        _currentBufferIndex = (_currentBufferIndex + 1) % MIC_BUFFER_MESSAGE_QUEUE_SIZE;

        if (!_rawMicDataMessageQueue->sendFromISR(&_micBuffMessage))
        {
            // _hardware->diag->error(TAG, "failed to put local mic data queue\n");
            // failed to post to the local queue. This means the main exec loop is held up and not popping them quick enough

            // Increment the buffer drop in the local metrics

            if (!_localBufferOverrun)
            {
                _localBufferOverrun = true;
                messages::Message m;
                m.type = messages::InternalMessageType::AUDIO_FRAME_RAW_BUFFER_OVERRUN;

                unsigned int _msgCount = 0;
                _execMessageQueue->getNumberOfQueuedItems(&_msgCount);
                // if this fails there's nothing we can do about it anyway..
                _execMessageQueue->sendFromISR(&m);
            }
        }
        else
        {
            _localBufferOverrun = false;
            messages::Message m;
            m.type = messages::InternalMessageType::AUDIO_FRAME_RAW;
            m.data.micRawData = NULL;

            if (!_execMessageQueue->sendFromISR(&m))
            {
                _hardware->diag->error(TAG, "failed to post main queue\n");
            }
        }
    }

}

void executive::micInterface::onError(ADC_AD7768::ADCError error)
{

}




//void executive::micInterface::onDMARxTxHalfComplete(void)
//{
//    // copy data into a buffer.
//    //_hardware->diag->info(TAG, "Half transfer complete\n");
//
//    // copy out the first half of the DMA buffer while the second half is being filled
//    memcpy(_micDataQueueBuffer[_currentBufferIndex].micData, pRxData, BUFF_LENGTH / 2);
//
//}
//
//void executive::micInterface::onDMARxTxComplete(void)
//{
//    static bool _localBufferOverrun = false;
//
//    _micDataQueueBuffer[_currentBufferIndex].header.messageLength = sizeof(messages::MicArrayRawDataMessage);
//    _micDataQueueBuffer[_currentBufferIndex].header.type = messages::ExternalMessageType::AUDIO_FRAME_RAW;
//    _micDataQueueBuffer[_currentBufferIndex].timeStamp = _bufferSequenceNumber;
//    _micDataQueueBuffer[_currentBufferIndex].sequenceNumber = _bufferSequenceNumber++;
//
//    // copy out the second half of the DMA buffer while the first half is being filled
//    memcpy((uint8_t*) (_micDataQueueBuffer[_currentBufferIndex].micData) + BUFF_LENGTH / 2, pRxData + BUFF_LENGTH / 2, BUFF_LENGTH / 2);
//
//    // post a message to the local queue, which is size-bound
//    messages::MicArrayRawDataMessage *_micBuffMessage;
//    _micBuffMessage = &_micDataQueueBuffer[_currentBufferIndex];
//    _currentBufferIndex = (_currentBufferIndex + 1) % MIC_BUFFER_MESSAGE_QUEUE_SIZE;
//
//    if (!_rawMicDataMessageQueue->sendFromISR(&_micBuffMessage))
//    {
////        _hardware->diag->error(TAG, "failed to put local mic data queue\n");
//        // failed to post to the local queue. This means the main exec loop is held up and not popping them quick enough
//
//        // Increment the buffer drop in the local metrics
//
//        if (!_localBufferOverrun)
//        {
//            _localBufferOverrun = true;
//            messages::Message m;
//            m.type = messages::InternalMessageType::AUDIO_FRAME_RAW_BUFFER_OVERRUN;
//
//            unsigned int _msgCount = 0;
//            _execMessageQueue->getNumberOfQueuedItems(&_msgCount);
//            // if this fails there's nothing we can do about it anyway..
//            _execMessageQueue->sendFromISR(&m);
//        }
//
//    }
//    else
//    {
//        _localBufferOverrun = false;
//        messages::Message m;
//        m.type = messages::InternalMessageType::AUDIO_FRAME_RAW;
//        m.data.micRawData = NULL;
//
//        if (!_execMessageQueue->sendFromISR(&m))
//        {
//            _hardware->diag->error(TAG, "failed to post main queue\n");
//        }
//    }
//
//}
