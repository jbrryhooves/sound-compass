/*
 * ADC_AD7768.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_ADC_AD7768_HPP_
#define INC_EXECUTIVE_ADC_AD7768_HPP_
//-------------------------------------------------------------------
// Module       : ADC_AD7768.hpp
// Description  :
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <array>
#include "stdint.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"
#include "platform/interfaces/IMessageQueue.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/ISPI.hpp"
#include "platform/interfaces/IQSPI.hpp"
#include "platform/interfaces/IHardwareTimer.hpp"

#include "executive/constants.hpp"
#include "executive/messages.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------



namespace executive
{

    class ADC_AD7768
    {
    public:
        struct AD7768Config
        {
            platform::ISPI *spiPort;
            platform::ISAI *sai;
            platform::IHardwareTimer *sampleRateTimer;
            platform::IGPIO *chipSelectPinMaster;
            platform::IGPIO *chipSelectPinSlave;
            platform::IGPIO *DRDYPin;
            platform::IGPIO *startPin;
        };

        typedef struct __attribute__((__packed__)) _ADCHeaderByte
        {
            bool Error;            // bit 7
            bool FilterNotSettled; // bit 6
            bool RepeatedData;     // bit 5
            bool FilterType;       // bit 4
            bool FilterSaturated;  // bit 3
            uint8_t ChannelID;     // bit 0-2

        } ADCHeaderByte;

//        typedef struct __attribute__((__packed__)) _ADCChannelSample
//        {
//            ADCHeaderByte header;
//            float value;
//        } ADCChannelSample;

        typedef struct __attribute__((__packed__)) _ADCSimultaneousSample
        {
            uint64_t timeStamp;
            uint32_t sequenceNumber;
            std::array<ADCHeaderByte, NUMBER_OF_MICS> sampleHeaders;
            std::array<float, NUMBER_OF_MICS> samples;

        } ADCSimultaneousSample;

        enum class ADCError
        {
            Ok,

        };

        class IADCListener
        {
        public:
            virtual void onSample(ADCSimultaneousSample &sample) = 0;
            virtual void onError(ADCError error) = 0;
        };

        bool initialise(board::IBoardHardware *hardware, AD7768Config config, IADCListener *adcListener);
        bool configure();
        bool start();
//
//        // ISPIListener
//        void onBytesReceived(uint8_t *buff, uint32_t len);
//        void onDMARxTxHalfComplete(void);
//        void onDMARxTxComplete(void);

        void _SAIBytesReceivedCallback(uint8_t saiChannel);
    private:
        typedef uint8_t register_t;


        typedef struct {
            uint8_t ReadWrite;
            register_t reg;
            uint8_t value;
        } registerSet;


        board::IBoardHardware *_hardware;
        AD7768Config _config;

        IADCListener *_adcListener;

        bool readRegister(executive::ADC_AD7768::registerSet &reg);
        bool readWriteRegister(executive::ADC_AD7768::registerSet &reg);
        // bool writeRegister(executive::ADC_AD7768::registerSet reg, uint8_t &value);

        static const registerSet checkRevisionRegister;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_ADC_AD7768_HPP_ */
