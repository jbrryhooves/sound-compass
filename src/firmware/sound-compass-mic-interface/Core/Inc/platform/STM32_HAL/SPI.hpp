/*
 * spi.hpp
 *
 *  Created on: Sep 21, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_STM32_HAL_SPI_HPP_
#define INC_PLATFORM_STM32_HAL_SPI_HPP_
//-------------------------------------------------------------------
// Module       : spi.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "platform/interfaces/ISPI.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::stm32hal
{
    class SPI: public platform::ISPI
    {
    public:
        bool initialise(SPI_HandleTypeDef *spiHandle, DMA_HandleTypeDef *spiDMAhandleRx, DMA_HandleTypeDef *spiDMAhandleTx);

        void transferHalfCompleteCallback(SPI_HandleTypeDef *hspi);
        void transferCompleteCallback(SPI_HandleTypeDef *hspi);

        // ISPI
        bool transmitReceiveDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t size);
        void registerListener(ISPIListener *listener);


    private:
        SPI_HandleTypeDef *_spiHandle;
        DMA_HandleTypeDef *_spiDMAhandleRx;
        DMA_HandleTypeDef *_spiDMAhandleTx;

        platform::ISPI::ISPIListener *_listener;
    };

}

#endif /* INC_PLATFORM_STM32_HAL_SPI_HPP_ */

