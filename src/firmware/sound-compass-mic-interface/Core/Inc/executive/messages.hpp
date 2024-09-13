/*
 * messages.hpp
 *
 *  Created on: Feb 12, 2024
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_MESSAGES_HPP_
#define INC_EXECUTIVE_MESSAGES_HPP_
//-------------------------------------------------------------------
// Module       : messages.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include "stdint.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "executive/constants.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
namespace executive
{

    class messages
    {
    public:

        static constexpr uint16_t BUFFER_LENGTH_SAMPLES = (uint16_t) (SAMPLE_RATE_Hz * (BUFFER_LENGTH_ms / 1000.0));

        enum class ExternalMessageType : uint8_t
        {
            AUDIO_FRAME_RAW,
            RUNTIME_METRICS
        };

        // The header at the beginning of every message sent or received from this application
        // Packed because it is sent over the wire.
        typedef struct __attribute__((__packed__)) _ExternalMessageHeader
        {
            const uint64_t messageHeaderDelimeter = 0xBEABEABEABEABEAB;
            uint32_t messageLength;
            ExternalMessageType type;

        } ExternalMessageHeader;

        // Holds raw mic data.
        // Message that is sent externally
        // Packed because it is sent over the wire
        typedef struct __attribute__((__packed__))
        {
            ExternalMessageHeader header;
            uint64_t timeStamp;
            uint32_t sequenceNumber;
            const uint32_t NUMBER_OF_MICS = NUMBER_OF_MICS;
            const uint32_t BUFFER_LENGTH_SAMPLES = BUFFER_LENGTH_SAMPLES;
            float micData[NUMBER_OF_MICS][BUFFER_LENGTH_SAMPLES];

        } MicArrayRawDataMessage;

        // Holds runtime stats. Packed because it is sent over the wire
        typedef struct __attribute__((__packed__))
        {
            ExternalMessageHeader header;
            uint64_t timeStamp;
            uint32_t upTime_s;
            uint32_t droppedRawBuffersSinceLastMessage;
            uint32_t droppedRawBuffersTotal;

        } RunTimeMetricsMessage;

        enum class InternalMessageType : uint8_t
        {
            QUIT,
            STATE_TRANSIT,
            STATE_TIMER,
            DEBUG_LED,
            AUDIO_FRAME_RAW,
            AUDIO_FRAME_RAW_BUFFER_OVERRUN,
            RUNTIME_METRICS
        };

        // The base message type for internal messages between tasks
        typedef struct
        {
            InternalMessageType type;
            union
            {
                MicArrayRawDataMessage *micRawData;
                RunTimeMetricsMessage *runtimeMetricsData;
            } data;
        } Message;


    };

} /* namespace executive */





#endif /* INC_EXECUTIVE_MESSAGES_HPP_ */

