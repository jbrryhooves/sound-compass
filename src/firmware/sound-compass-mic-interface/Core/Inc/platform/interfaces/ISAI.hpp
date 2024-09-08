/*
 * ISPI.hpp
 *
 *  Created on: Sep 21, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_ISAI_HPP_
#define INC_PLATFORM_INTERFACES_ISAI_HPP_
//-------------------------------------------------------------------
// Module       : IQSPI.hpp
// Description  :
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdint.h>

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
    class ISAI
    {
    public:
        /// @brief callback method signature
        /// @param channel
        using bytesReceivedCallback_t = void(uint8_t);

        /**
         * @brief
         *
         * @param
         * @param
         */
        virtual bool receiveDMA(uint8_t *pRxData_A, uint8_t *pRxData_B, uint16_t size) = 0;
        virtual void registerCallback(bytesReceivedCallback_t *callback) = 0;
    };

}

#endif /* INC_PLATFORM_INTERFACES_IQSPI_HPP_ */
