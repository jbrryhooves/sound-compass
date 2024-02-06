/*
 * tcpServer.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_TCPSERVER_HPP_
#define INC_EXECUTIVE_TCPSERVER_HPP_
//-------------------------------------------------------------------
// Module       : tcpServer.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"
#include "board/interfaces/IBoardHardware.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class tcpServer: public platform::ITask
    {
    public:

        typedef struct _txByteArrayMessage
        {
            uint32_t arrayLength;
            uint8_t *array;

        } txByteArrayMessage;

        enum class TCPError
        {
            Ok,
            NoClientConnected,
            Timeout,
            InitFailure
        };

        class ITCPServerListener
        {
        public:
            virtual void onControlCommandReceived(void) = 0;
            virtual void onServerError(TCPError error) = 0;
        };

        bool initialise(board::IBoardHardware *hardware, ITCPServerListener *tcpServerListener, platform::ITaskFactory *taskFactory);
        bool start();

        TCPError transmitByteArray(txByteArrayMessage txMessage);

        // ITask
        void taskMain(void);
        bool killEnable(bool enable);

    private:

//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;
        platform::ITaskFactory *_taskFactory;
        platform::ITaskFactory::TaskHandle _taskHandle;
        ITCPServerListener *_tcpServerListener;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_TCPSERVER_HPP_ */

