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

        enum class MessageType
        {
            QUIT,
            STATE_TRANSIT,
            STATE_TIMER,
            DEBUG_LED,
            AUDIO_FRAME_RAW,
            AUDIO_FRAME_PROCESSED,
            AUDIO_FRAME_METRICS
        };

        // A struct to hold raw mic data
        typedef struct
        {
            uint64_t timeStamp;
            uint32_t micData[BUFFER_LENGTH_SAMPLES][NUMBER_OF_MICS];

        } MicArrayRawDataMessage;

        typedef struct
        {
            MessageType type;
            union
            {
                MicArrayRawDataMessage *micRawData;
            } data;
        } Message;


    };

} /* namespace executive */





#endif /* INC_EXECUTIVE_MESSAGES_HPP_ */

