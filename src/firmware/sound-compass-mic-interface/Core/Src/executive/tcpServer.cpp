//-------------------------------------------------------------------
// Module       : audioProcessor.cpp
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

#include "executive/tcpServer.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char *TAG = "tcpServer";

osThreadId_t tcpServerTaskHandle;
uint32_t tcpServerTaskBuffer[1024];
StaticTask_t tcpServerTaskControlBlock;
const osThreadAttr_t tcpServerTask_attributes = { .name = "tcpServerTask", .cb_mem = &tcpServerTaskControlBlock, .cb_size = sizeof(tcpServerTaskControlBlock), .stack_mem =
        &tcpServerTaskBuffer[0], .stack_size = sizeof(tcpServerTaskBuffer),
        .priority = (osPriority_t) osPriorityNormal1, };

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------


// @formatter:off
// @formatter:on
// =============================================================


bool executive::tcpServer::initialise(board::IBoardHardware *hardware, ITCPServerListener *tcpServerListener, platform::ITaskFactory *taskFactory)
{
    _hardware = hardware;
    _tcpServerListener = tcpServerListener;
    _taskFactory = taskFactory;

    _hardware->diag->info(TAG, "TCP Server init\n");
    return true;
}


bool executive::tcpServer::start()
{
    _hardware->diag->info(TAG, "Starting..\n");

    _taskHandle = _taskFactory->createTaskStatic("tcpServerTask", &tcpServerTaskControlBlock, sizeof(tcpServerTaskControlBlock), &tcpServerTaskBuffer[0],
            sizeof(tcpServerTaskBuffer), osPriorityAboveNormal, this /* ITask*/);

    if (_taskHandle == nullptr)
    {
        _hardware->diag->error(TAG, "Can not create tcpServerTask task");
        return false;
    }
    return true;
}

executive::tcpServer::TCPError executive::tcpServer::sendBytes(uint8_t *data, size_t length, int8_t *internalError)
{
    if (_newClientConnection != NULL)
    {
        if (err_t ret = netconn_write(_newClientConnection, data, length, NETCONN_COPY); ret != 0)
        {
            *internalError = (int8_t) ret;
            return executive::tcpServer::TCPError::LwIPFailure;
        }
    }
    return executive::tcpServer::TCPError::Ok;
}


// ITask
void executive::tcpServer::taskMain(void)
{
    static struct netconn *conn;
    static struct netbuf *buf;
    static ip_addr_t *clientIPAddress;
    static unsigned short clientPort;

    static char msg[100];
    static char smsg[200];

    err_t err, accept_err, recv_error;

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL)
    {
        /* Bind connection to the port number 7000. */
        err = netconn_bind(conn, IP_ADDR_ANY, 7000);

        if (err == ERR_OK)
        {
            /* Tell connection to go into listening mode. */
            netconn_listen(conn);

            while (1)
            {
                /* Grab new connection. */
                accept_err = netconn_accept(conn, &_newClientConnection);

                /* Process the new connection. */
                if (accept_err == ERR_OK)
                {
                    int len = sprintf(smsg, "welcome \n");

                    netconn_write(_newClientConnection, smsg, len, NETCONN_COPY);  // send the message back to the client

                    /* receive the data from the client */
                    while (netconn_recv(_newClientConnection, &buf) == ERR_OK)
                    {
                        /* Extrct the address and port in case they are required */
                        clientIPAddress = netbuf_fromaddr(buf);  // get the address of the client
                        clientPort = netbuf_fromport(buf);  // get the Port of the client

                        /* If there is some data remaining to be sent, the following process will continue */
                        do
                        {
                            strncpy(msg, (const char*) buf->p->payload, buf->p->len);   // get the message from the client

                            _tcpServerListener->onControlCommandReceived();

                            // Or modify the message received, so that we can send it back to the client
                            int len = sprintf(smsg, "\"%s\" was sent by the Server\n", msg);

                            netconn_write(_newClientConnection, smsg, len, NETCONN_COPY);  // send the message back to the client
                            memset(msg, '\0', 100);  // clear the buffer
                        } while (netbuf_next(buf) > 0);

                        netbuf_delete(buf);
                    }

                    /* Close connection and discard connection identifier. */
                    netconn_close(_newClientConnection);
                    netconn_delete(_newClientConnection);
                    _newClientConnection = NULL;
                }
            }
        }
        else
        {
            DIAG_LINE_ERROR(_hardware->diag, TAG, "Failed to start TCP Server. Err: %d\n", err);
            _tcpServerListener->onServerError(executive::tcpServer::TCPError::InitFailure);
            netconn_delete(conn);
        }
    }
    

}

bool executive::tcpServer::killEnable(bool enable)
{

}
