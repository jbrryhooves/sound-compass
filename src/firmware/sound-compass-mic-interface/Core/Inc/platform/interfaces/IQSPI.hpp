/*
 * ISPI.hpp
 *
 *  Created on: Sep 21, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IQSPI_HPP_
#define INC_PLATFORM_INTERFACES_IQSPI_HPP_
//-------------------------------------------------------------------
// Module       : IQSPI.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{
    class IQSPI
    {
    public:

        class IQSPIListener
        {
        public:
            virtual void onBytesReceived(uint8_t *buff, uint32_t len) = 0;
            virtual void onDMARxTxHalfComplete(void) = 0;
            virtual void onDMARxTxComplete(void) = 0;
        };

        /**
         * @brief
         *
         * @param
         * @param
         */
        virtual bool transmitReceiveDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t size) = 0;
        virtual void registerListener(IQSPIListener *listener) = 0;



    };

}

#endif /* INC_PLATFORM_INTERFACES_IQSPI_HPP_ */

