//-------------------------------------------------------------------
// Module       : ADC_AD7768.cpp
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

#include <array>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "executive/constants.hpp"
#include "executive/ADC_AD7768.hpp"
#include "util/assert.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char *TAG = "ADC_AD7768";

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
#define DMA_BUFFER __attribute__((section(".RxMicDataSection")))
#define BUFFER_ALIGNED_SIZE (((BUFF_LENGTH + 31) / 32) * 32)

//#define BUFF_LENGTH 48 //0x7800
// 0x7800 = data for 10ms of data for 16 mics at 48kHz sample rate
// 16 (mics) x 4 (bytes/mic/sample) * 48000 * 0.01

// Receive one sample per mic.
// = NUMBER_OF_MICS / SAI_OUTPUT_CHANNELS
static constexpr uint16_t SAI_BLOCK_DMA_BUFF_LENGTH_samples = (NUMBER_OF_MICS / 2) / 2; // extra divide by 2 for dev board temporarily only having one ADC



static __attribute__((section(".sai_dma_buffer_section")))   __attribute__((aligned(32)))     std::array<uint32_t, SAI_BLOCK_DMA_BUFF_LENGTH_samples> _saiChanARxBuff;
static __attribute__((section(".sai_dma_buffer_section")))   __attribute__((aligned(32)))     std::array<uint32_t, SAI_BLOCK_DMA_BUFF_LENGTH_samples> _saiChanBRxBuff;

float convertADCSample(uint32_t rawSample, executive::ADC_AD7768::ADCHeaderByte &headerByte);
extern "C"
{
void _SAIBytesReceivedCallback_wrapper(uint8_t saiChannel);

}

// @formatter:off
// @formatter:on
// =============================================================

//static const std::array<executive::ADC_AD7768::registerSet, 2> checkRevision = {
//    {executive::ADC_AD7768::READ, executive::ADC_AD7768::REG_RevisionID, 0x00},
//    {executive::ADC_AD7768::READ, executive::ADC_AD7768::REG_RevisionID, 0x00},
//};

const static uint8_t READ = 0b10000000;
const static uint8_t WRITE = 0b00000000;

const static register_t REG_ChannelModeA            = 0x01;
const static register_t REG_PowerMode               = 0x04;
const static register_t REG_RevisionID              = 0x0A;
const static register_t REG_DataControl             = 0x06;
const static register_t REG_InterfaceConfiguration  = 0x07;

static executive::ADC_AD7768 *_instance;

bool executive::ADC_AD7768::initialise(board::IBoardHardware *hardware, AD7768Config config, IADCListener *adcListener)
{
    _hardware = hardware;
    _adcListener = adcListener;
    _config = config;

//    _config.spiPort->registerListener(/*ISPIListener*/ this);
//    _config.sai->registerCallback((platform::ISAI::bytesReceivedCallback_t*) (&executive::ADC_AD7768:: _SAIBytesReceivedCallback));
    _config.sai->registerCallback((platform::ISAI::bytesReceivedCallback_t*) (&_SAIBytesReceivedCallback_wrapper));

    // register GPIO interrupts
//    _config.DRDYPin->registerCallback((platform::IGPIO::interruptCallback_t*) (&executive::ADC_AD7768::_DRDYCallback));

    _hardware->diag->info(TAG, "AD7768 init complete\n");
    _instance = this;
    return true;
}

bool executive::ADC_AD7768::configure()
{
    _hardware->diag->info(TAG, "Configuring.. \n");
    
    // perform a full reset. 
    // Write to the DataControl register twice to perform the reset. 
    std::array<executive::ADC_AD7768::registerSet, 2> _resetCommands = { {
        { WRITE, REG_DataControl, 0b00000011 }, // first reset command
            { WRITE, REG_DataControl, 0b00000010 }  // first reset command
    } };

    for (auto r : _resetCommands)
    {
        if (!readWriteRegister(r))
        {
            _hardware->diag->error(TAG, "Failed to write ADC reset reg\n");
            return  false;
        }
    }

    executive::ADC_AD7768::registerSet _reg = { READ, REG_DataControl, 0x00 };
    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }

    // the ADC returns 0x0E00, but we're only reading the second byte of 0x00
    if(_reg.value !=  0x00)
    {
        _hardware->diag->error(TAG, "Failed to reset ADC\n");
        return false;
    }
    // Wait for the ADC Startup time after reset
    osDelay(200); // the maximum time is 1.66ms



    // ADC is now reset, so now we configure it
    
    // ------------------------------------------------------
    _reg = { READ, REG_ChannelModeA, 0x00 };
    if (!readRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    _reg.ReadWrite = WRITE;
    _reg.value &= ~0b00001000; // Filter type = Wideband filter
    _reg.value &= ~0b00000111; // 
    _reg.value |=  0b00000010; // Decimation rate = x128
    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    // -----------------------------------------------------

    // ------------------------------------------------------
    _reg = { READ, REG_PowerMode, 0x00 };
    if (!readRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    _reg.ReadWrite = WRITE;
    _reg.value |= 0b00011000; // Fast mode
    _reg.value |= 0b00000011; // Set MCLK/4
    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    // -----------------------------------------------------


    // -----------------------------------------------------
    _reg = { READ, REG_DataControl, 0x00 };
    if (!readRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    _reg.ReadWrite = WRITE;
    _reg.value |= 0b00010000; // Enable SINGLE_SHOT_MODE
    
//    if (!readWriteRegister(_reg))
//    {
//        _hardware->diag->error(TAG, "Failed to write ADC DataControl\n");
//    }
    // -----------------------------------------------------

    // -----------------------------------------------------
    _reg = { READ, REG_InterfaceConfiguration, 0x00 };
    if (!readRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    _reg.ReadWrite = WRITE;
    // _reg.value |= 0b00000010; // Set DCLK div to 2. So, the final DCLK rate is MCLK/2 = 16.384 MHz
    _reg.value |= 0b00000001; // Set DCLK div to 4. So, the final DCLK rate is MCLK/4 = 8.192 MHz

    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
    }
    // -----------------------------------------------------

    return true;
}

bool executive::ADC_AD7768::start()
{
    _hardware->diag->info(TAG, "Starting..\n");

    bool success = true;

    // Start the ADC conversions by sending a SPI_SYNC command,
    // which is just clearing then setting the SPI_SYNC bit
    executive::ADC_AD7768::registerSet _reg ;

    _reg = { READ, REG_DataControl, 0x00 };
    if (!readRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to read ADC DataControl\n");
        success = false;
    }
    _reg.ReadWrite = WRITE;
    _reg.value &= ~0b10000000; // Clear SPI_SYNC bit
    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to write ADC DataControl\n");
        success = false;
    }
    
    _reg.value |= 0b10000000; // Set SPI_SYNC bit
    if (!readWriteRegister(_reg))
    {
        _hardware->diag->error(TAG, "Failed to write ADC DataControl\n");
        success = false;
    }
    
    // DMA channels are set up in circular mode, so start the receiving here
    if (!_config.sai->receiveDMA((uint8_t*) _saiChanARxBuff.data(), (uint8_t*) _saiChanBRxBuff.data(), SAI_BLOCK_DMA_BUFF_LENGTH_samples))
    {
        _hardware->diag->error(TAG, "Error while starting the SAI DMA receiving");
        success = false;
    }

    return success;
}

//========================================
// =========== Private ====================

/// @brief Read a register byte. Performs 2 reads as per ADC SPI operation
/// @param reg 
/// @return 
bool executive::ADC_AD7768::readRegister(executive::ADC_AD7768::registerSet &reg)
{

    uint8_t txData[2] = { (uint8_t) (reg.reg | reg.ReadWrite), reg.value };
    uint8_t rxData[2] = {0};

    if (!_config.spiPort->transmitReceive(txData, rxData, 2, _config.chipSelectPinMaster))
    {
        _hardware->diag->error(TAG, "Failed to transmit");
        return false;
    }

    rxData[2] = { 0 };
    if (!_config.spiPort->transmitReceive(txData, rxData, 2, _config.chipSelectPinMaster))
    {
        _hardware->diag->error(TAG, "Failed to transmit");
        return false;
    }

    reg.value = rxData[1];
    return true;
}

bool executive::ADC_AD7768::readWriteRegister(executive::ADC_AD7768::registerSet &reg)
{

    uint8_t txData[2] = { (uint8_t) (reg.reg | reg.ReadWrite), reg.value };
    uint8_t rxData[2] = {0};

    if (!_config.spiPort->transmitReceive(txData, rxData, 2, _config.chipSelectPinMaster))
    {
        _hardware->diag->error(TAG, "Failed to transmit");
        return false;
    }

    reg.value = rxData[1];
    return true;
}

// bool executive::ADC_AD7768::writeRegister(executive::ADC_AD7768::registerSet reg, uint8_t &value)
// {
//     _hardware->diag->info(TAG, "Configuring.. \n");

//     uint8_t txData[2] = {reg.reg, reg.value};
//     uint8_t rxData[2] = {0};

//     if (!_config.spiPort->transmitReceive(txData, rxData, 2, _config.chipSelectPinMaster))
//     {
//         _hardware->diag->error(TAG, "Failed to transmit");
//         return false;
//     }
    
//     value = rxData[1];
//     return true;
// }

//========================================
// =======================================

// ===========================



//void executive::ADC_AD7768::onDMARxTxComplete(void)
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
//}


// ================ Private ===========

float convertADCSample(uint32_t rawSample, executive::ADC_AD7768::ADCHeaderByte &header)
{
    int32_t signedRawSample = rawSample & 0xFFFFFF;

    if (rawSample & 0x800000)
    {
        signedRawSample -= 0xFFFFFF;
    }

    float sample = (signedRawSample) / (float) 0x7FFFFF;

    uint8_t headerByte = (uint8_t) (rawSample >> 24);

    // @formatter:off
    header.Error             = ((headerByte >> 0x7) & 0x1)     == 0x1;
    header.FilterNotSettled  = ((headerByte >> 0x6) & 0x1)     == 0x1;
    header.RepeatedData      = ((headerByte >> 0x5) & 0x1)     == 0x1;
    header.FilterType        = ((headerByte >> 0x4) & 0x1)     == 0x1;
    header.FilterSaturated   = ((headerByte >> 0x3) & 0x1)     == 0x1;
    header.ChannelID         = ((headerByte       ) & 0b111);
        // @formatter:on

    return sample;
}

extern "C" void _SAIBytesReceivedCallback_wrapper(uint8_t saiChannel)
{
    if (_instance != nullptr)
    {
        _instance->_SAIBytesReceivedCallback(saiChannel);
    }
}

void executive::ADC_AD7768::_SAIBytesReceivedCallback(uint8_t saiChannel)
{

    static ADCSimultaneousSample arraySampleSet;
    static bool channelAReceived = false;
    static bool channelBReceived = false;
    

    ADCHeaderByte header;
    if (_instance != nullptr)
    {

        if (!channelAReceived && !channelBReceived)
        {
            arraySampleSet.timeStamp++; // TODO: get timestamp
            arraySampleSet.sequenceNumber++;
        }

        if (saiChannel == 0)
        {
            SCB_InvalidateDCache_by_Addr((uint32_t*) (_saiChanARxBuff.data()), sizeof(_saiChanARxBuff));

            for (uint16_t i = 0; i < _saiChanARxBuff.size(); i++)
            {
                arraySampleSet.samples[i] = convertADCSample(_saiChanARxBuff[i], header);
                arraySampleSet.sampleHeaders[i] = header;

            }
            channelAReceived = true;
        }
        else
        {
            SCB_InvalidateDCache_by_Addr((uint32_t*) (_saiChanBRxBuff.data()), sizeof(_saiChanBRxBuff));

            for (uint16_t i = 0; i < _saiChanBRxBuff.size(); i++)
            {
                arraySampleSet.samples[i + _saiChanBRxBuff.size()] = convertADCSample(_saiChanBRxBuff[i], header);
                arraySampleSet.sampleHeaders[i + _saiChanBRxBuff.size()] = header;
            }
            channelBReceived = true;
        }

        if (channelAReceived && channelBReceived)
        {
            _instance->_adcListener->onSample(arraySampleSet);
            channelAReceived = false;
            channelBReceived = false;

        }
    }
}

