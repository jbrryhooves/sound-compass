//-------------------------------------------------------------------
// Module       : tcpserver.c
// Description  :
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include "cmsis_os.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "string.h"
#include "queue.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "tcp-server.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

#define ARRAY_SAMPLE_BUFFER_ARRAY_SIZE      2
typedef struct _newSample
{
    uint32_t timeStamp;
    uint32_t count;

    float sample[16][960]; // 906 = 48000 * .02 (sample rate * time window)
} newSample;

static newSample sampleQueue[ARRAY_SAMPLE_BUFFER_ARRAY_SIZE];

static struct netconn *conn, *newconn;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;
char msg[100];
char smsg[200];

static void tcp_receive_thread_method(void *arg);
static void tcp_transmit_thread_method(void *arg);

osThreadId_t tcpRxTaskHandle;
uint32_t tcpRxTaskBuffer[512];
StaticTask_t tcpRxTaskControlBlock;
const osThreadAttr_t tcpRxTask_attributes = {
        .name = "tcpRxTask",
        .cb_mem = &tcpRxTaskControlBlock,
        .cb_size = sizeof(tcpRxTaskControlBlock),
        .stack_mem = &tcpRxTaskBuffer[0],
        .stack_size = sizeof(tcpRxTaskBuffer),
        .priority = (osPriority_t) osPriorityNormal, };

osThreadId_t tcpTxTaskHandle;
uint32_t tcpTxTaskBuffer[512];
StaticTask_t tcpTxTaskControlBlock;
const osThreadAttr_t tcpTxTask_attributes = {
        .name = "tcpTxTask",
        .cb_mem = &tcpTxTaskControlBlock,
        .cb_size = sizeof(tcpTxTaskControlBlock),
        .stack_mem = &tcpTxTaskBuffer[0],
        .stack_size = sizeof(tcpTxTaskBuffer),
        .priority = (osPriority_t) osPriorityNormal1, };


osMessageQueueId_t NewSampleQueueHandle;
uint8_t NewSampleQueueBuffer[16 * sizeof(newSample*)];
StaticQueue_t NewSampleQueueControlBlock;
const osMessageQueueAttr_t NewSampleQueue_attributes = { .name = "NewSampleQueue", .cb_mem = &NewSampleQueueControlBlock, .cb_size = sizeof(NewSampleQueueControlBlock), .mq_mem =
        &NewSampleQueueBuffer, .mq_size = sizeof(NewSampleQueueBuffer) };


bool generate_sample(void)
{
    static uint8_t index;
    static uint32_t count;
    newSample *pMessage;

    sampleQueue[index].count = count;

    pMessage = &sampleQueue[index];
    if (xQueueSend(NewSampleQueueHandle, (void*) &pMessage, (TickType_t) 0) != pdPASS)
    {
        return false;
    }

    count++;
    index = (index + 1) % ARRAY_SAMPLE_BUFFER_ARRAY_SIZE;
    return true;
}


bool tcpserver_init(void)
{
//    sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);

//    tcp_receive_thread_method(NULL)

    sampleQueue[0].timeStamp = 0;
    sampleQueue[1].timeStamp = 1;
    NewSampleQueueHandle = osMessageQueueNew(16, sizeof(newSample*), &NewSampleQueue_attributes);

    tcpRxTaskHandle = osThreadNew(tcp_receive_thread_method, NULL, &tcpRxTask_attributes);
    tcpTxTaskHandle = osThreadNew(tcp_transmit_thread_method, NULL, &tcpTxTask_attributes);

    return true;
}

static void tcp_transmit_thread_method(void *arg)
{
    newSample *rxRxedPointer;

    while (1)
    {
//        uxQueueSpacesAvailable( QueueHandle_t xQueue );
        if (xQueueReceive(NewSampleQueueHandle, &(rxRxedPointer), (TickType_t) 10) == pdPASS)
        {
            /* xRxedStructure now contains a copy of xMessage. */
            if (newconn != NULL)
            {
                int len = sprintf(smsg, "sample %u: index: %u\n", rxRxedPointer->count, rxRxedPointer->timeStamp);

                netconn_write(newconn, smsg, len, NETCONN_COPY);  // send the message back to the client
            }
        }
    }
}

/**** Send RESPONSE every time the client sends some data ******/
static void tcp_receive_thread_method(void *arg)
{
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
                accept_err = netconn_accept(conn, &newconn);

                /* Process the new connection. */
                if (accept_err == ERR_OK)
                {
                    int len = sprintf(smsg, "welcome \n");

                    netconn_write(newconn, smsg, len, NETCONN_COPY);  // send the message back to the client


                    /* receive the data from the client */
                    while (netconn_recv(newconn, &buf) == ERR_OK)
                    {
                        /* Extrct the address and port in case they are required */
                        addr = netbuf_fromaddr(buf);  // get the address of the client
                        port = netbuf_fromport(buf);  // get the Port of the client

                        /* If there is some data remaining to be sent, the following process will continue */
                        do
                        {
                            strncpy(msg, buf->p->payload, buf->p->len);   // get the message from the client

                            // Or modify the message received, so that we can send it back to the client
                            int len = sprintf(smsg, "\"%s\" was sent by the Server\n", msg);

                            netconn_write(newconn, smsg, len, NETCONN_COPY);  // send the message back to the client
                            memset(msg, '\0', 100);  // clear the buffer
                        } while (netbuf_next(buf) > 0);

                        netbuf_delete(buf);
                    }

                    /* Close connection and discard connection identifier. */
                    netconn_close(newconn);
                    netconn_delete(newconn);
                    newconn = NULL;
                }
            }
        }
        else
        {
            netconn_delete(conn);
        }
    }
}
