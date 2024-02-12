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
#define DMA_BUFFER __attribute__((section(".RxMicDataSection")))

#define BUFF_LENGTH 0x7800
// 0x7800 = data for 10ms of data for 16 mics at 48kHz sample rate
// 16 (mics) x 4 (bytes/mic/sample) * 48000 * 0.01

#define BUFFER_ALIGNED_SIZE (((BUFF_LENGTH+31)/32)*32)
DMA_BUFFER ALIGN_32BYTES(uint8_t pRxData[BUFFER_ALIGNED_SIZE]);

DMA_BUFFER static uint8_t pTxData[BUFFER_ALIGNED_SIZE];


// @formatter:off
// @formatter:on
// =============================================================


bool executive::micInterface::initialise(board::IBoardHardware *hardware, IMicInterfaceListener *micInterfaceListener, platform::IMessageQueue *rawMicDataQueue)
{
    _hardware = hardware;
    _micInterfaceListener = micInterfaceListener;
    _messageQueue = rawMicDataQueue;

    _spiPort = _hardware->SPI;

    _spiPort->registerListener(/*ISPIListener*/this);

    memset(_micDataBuffer, 0, sizeof(_micDataBuffer));


    _hardware->diag->info(TAG, "Mic Interface init\n");
    return true;
}


bool executive::micInterface::start()
{
    _hardware->diag->info(TAG, "Starting..\n");

    memset(pRxData, 0, BUFFER_ALIGNED_SIZE);

    for (uint32_t i = 0; i < executive::messages::BUFFER_LENGTH_SAMPLES; i++)
    {
        for (uint32_t m = 0; m < NUMBER_OF_MICS; m++)
        {
            uint32_t index = i * NUMBER_OF_MICS * sizeof(uint32_t) + m * sizeof(uint32_t);
            uint32_t val = i + m;
            pTxData[index] = (val >> 0) & 0xFF;
            pTxData[index + 1] = (val >> 8) & 0xFF;
            pTxData[index + 2] = (val >> 16) & 0xFF;
            pTxData[index + 3] = (val >> 24) & 0xFF;
        }

    }

    if (!_spiPort->transmitReceiveDMA(pTxData, pRxData, BUFF_LENGTH))
    {
        _hardware->diag->error(TAG, "Failed to transmit");

    }
//    _hardware->diag->error(TAG, "Can not create tcpServerTask task");

    return true;
}

void executive::micInterface::onBytesReceived(uint8_t *buff, uint32_t len)
{

}
void executive::micInterface::onDMATxBuffEmpy(void)
{
    // not used
}
void executive::micInterface::onDMARxTxHalfComplete(void)
{
    pTxData[0]++;

    // copy data into a buffer.
    //_hardware->diag->info(TAG, "Half transfer complete\n");
}
void executive::micInterface::onDMARxTxComplete(void)
{
    pTxData[BUFF_LENGTH / 2]++;

    // copy data into a buffer, and add the address of the buffer to a queue.
    // change the location of the "current" buffer.
    //_hardware->diag->info(TAG, "transfer complete\n");


    memcpy(_micDataBuffer[_currentBufferIndex].micData, pRxData, BUFF_LENGTH);

    messages::Message m;
    m.type = messages::MessageType::AUDIO_FRAME_RAW;
    m.data.micRawData = &_micDataBuffer[_currentBufferIndex];
    if (!_messageQueue->sendFromISR(&m))
    {
        _hardware->diag->error(TAG, "failed to put queue\n");

    }

    _currentBufferIndex = (_currentBufferIndex + 1) % MIC_BUFFER_MESSAGE_QUEUE_SIZE;
}
