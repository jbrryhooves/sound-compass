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

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class micInterface: public platform::ISPI::ISPIListener
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

        // ISPIListener
        void onBytesReceived(uint8_t *buff, uint32_t len);
        void onDMARxTxHalfComplete(void);
        void onDMARxTxComplete(void);


        static const uint8_t MIC_BUFFER_MESSAGE_QUEUE_SIZE = 2;

    private:

//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;

        IMicInterfaceListener *_micInterfaceListener;
        platform::IMessageQueue *_execMessageQueue;

        platform::ISPI *_spiPort;

        uint32_t _bufferSequenceNumber;
        // store the actual raw data here
        messages::MicArrayRawDataMessage _micDataQueueBuffer[MIC_BUFFER_MESSAGE_QUEUE_SIZE];
        uint8_t _currentBufferIndex = 0;
        platform::IMessageQueue *_rawMicDataMessageQueue;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_TCPSERVER_HPP_ */

