/*
 * spi.hpp
 *
 *  Created on: Sep 21, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_STM32_HAL_SAI_HPP_
#define INC_PLATFORM_STM32_HAL_SAI_HPP_
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
#include "platform/interfaces/ISAI.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform::stm32hal
{
    class SAI : public platform::ISAI
    {
    public:
        bool initialise(SAI_HandleTypeDef *spiHandleA, SAI_HandleTypeDef *spiHandleB, DMA_HandleTypeDef *spiDMAhandleRxA, DMA_HandleTypeDef *spiDMAhandleRxB);

        // ISAI
        bool receiveDMA(uint8_t *pRxData_A, uint8_t *pRxData_B, uint16_t size);
        void registerCallback(bytesReceivedCallback_t *callback);

        void transferHalfCompleteCallback(SAI_HandleTypeDef *hspi);
        void transferCompleteCallback(SAI_HandleTypeDef *hspi);

    private:
        SAI_HandleTypeDef *_saiHandleA;
        SAI_HandleTypeDef *_saiHandleB;
        DMA_HandleTypeDef *_saiDMAhandleRxA;
        DMA_HandleTypeDef *_saiDMAhandleRxB;

        platform::ISAI::bytesReceivedCallback_t *_callback;
    };

}

#endif /* INC_PLATFORM_STM32_HAL_SPI_HPP_ */
