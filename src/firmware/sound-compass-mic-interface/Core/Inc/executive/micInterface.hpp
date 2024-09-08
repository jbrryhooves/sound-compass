/*
 * tcpServer.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_MIC_INTERFACE_HPP_
#define INC_EXECUTIVE_MIC_INTERFACE_HPP_
//-------------------------------------------------------------------
// Module       : micInterface.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"
#include "platform/interfaces/IMessageQueue.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/ISPI.hpp"

#include "executive/messages.hpp"
#include "executive/ADC_AD7768.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class micInterface: ADC_AD7768::IADCListener
    {
    public:

        enum class MicInterfaceError
        {
            Ok,

        };

        class IMicInterfaceListener
        {
        public:
            virtual void onError(MicInterfaceError error) = 0;
        };

        bool initialise(board::IBoardHardware *hardware, IMicInterfaceListener *micInterfaceListener, platform::IMessageQueue *rawMicDataQueue);
        bool start();

        bool popMicDataMessage(messages::MicArrayRawDataMessage **message);

        // ADC_AD7768::IADCListener
        void onSample(ADC_AD7768::ADCSimultaneousSample sample);
        void onError(ADC_AD7768::ADCError error);

        static const uint8_t MIC_BUFFER_MESSAGE_QUEUE_SIZE = 2;

    private:

//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;

        IMicInterfaceListener *_micInterfaceListener;
        platform::IMessageQueue *_execMessageQueue;


        uint32_t _bufferSequenceNumber;
        // store the actual raw data here
        messages::MicArrayRawDataMessage _micDataQueueBuffer[MIC_BUFFER_MESSAGE_QUEUE_SIZE];
        uint8_t _currentBufferIndex = 0;
        platform::IMessageQueue *_rawMicDataMessageQueue;

        executive::ADC_AD7768 _ad7768;


    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_TCPSERVER_HPP_ */

