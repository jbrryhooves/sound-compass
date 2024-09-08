/*
 * ISPI.hpp
 *
 *  Created on: Sep 21, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_ISPI_HPP_
#define INC_PLATFORM_INTERFACES_ISPI_HPP_
//-------------------------------------------------------------------
// Module       : ISPI.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/IGPIO.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class ISPI
    {
    public:

        class ISPIListener
        {
        public:
            virtual void onBytesReceived(uint8_t *buff, uint32_t len) = 0;
            virtual void onDMARxTxHalfComplete(void) = 0;
            virtual void onDMARxTxComplete(void) = 0;
        };

        /**
         * @brief transmitReceiveDMA
         *
         * @param
         * @param
         */
        virtual bool transmitReceiveDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t size) = 0;

        /**
         * @brief
         *
         * @param
         * @param
         */
        virtual bool transmitReceive(const uint8_t *pTxData, uint8_t *pRxData, uint16_t size, IGPIO *chipSelectPin) = 0;

        /**
         * @brief
         *
         * @param
         * @param
         */
        virtual void registerListener(ISPIListener *listener) = 0;



    };

}

#endif /* INC_PLATFORM_INTERFACES_ISPI_HPP_ */

