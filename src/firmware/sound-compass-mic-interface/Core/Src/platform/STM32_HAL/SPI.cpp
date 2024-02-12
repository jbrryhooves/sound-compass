//-------------------------------------------------------------------
// Module       : SPI.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include <platform/STM32_HAL/SPI.hpp>
#include "util/assert.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

static platform::stm32hal::SPI *_spiInstance = nullptr;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platform::stm32hal::SPI::initialise(SPI_HandleTypeDef *spiHandle, DMA_HandleTypeDef *spiDMAhandleRx, DMA_HandleTypeDef *spiDMAhandleTx)
{
    // for now, until we have several SPI ports, this SPI instance must be a singleton. If another instance has been created, then we need to do something different
    ASSERT(_spiInstance == nullptr);
    _spiInstance = this;

    _spiHandle = spiHandle;
    _spiDMAhandleRx = spiDMAhandleRx;
    _spiDMAhandleTx = spiDMAhandleTx;

    return true;
}

void platform::stm32hal::SPI::transferHalfCompleteCallback(SPI_HandleTypeDef *hspi)
{
    if (_listener != nullptr)
    {
        _listener->onDMARxTxHalfComplete();
    }
}

void platform::stm32hal::SPI::transferCompleteCallback(SPI_HandleTypeDef *hspi)
{
    if (_listener != nullptr)
    {
        _listener->onDMARxTxComplete();
    }
}

//-------------------------------------------------------------------
// Public ISPI
//-------------------------------------------------------------------
void platform::stm32hal::SPI::registerListener(platform::ISPI::ISPIListener *listener)
{
    _listener = listener;
}


bool platform::stm32hal::SPI::transmitReceiveDMA(const uint8_t *pTxData, uint8_t *pRxData, uint16_t size)
{

    return HAL_SPI_TransmitReceive_DMA(_spiHandle, pTxData, pRxData, size) == HAL_OK;
//    return HAL_SPI_TransmitReceive(_spiHandle, pTxData, pRxData, size, 1000) == HAL_OK;
}

//void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);
//void HAL_SPI_SuspendCallback(SPI_HandleTypeDef *hspi);

//-------------------------------------------------------------------
// STM32 HAL C Callbacks
//-------------------------------------------------------------------


/**
 * @brief  TxRx Transfer completed callback.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report end of DMA TxRx transfer, and
 *         you can add your own implementation.
 * @retval None
 */

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    _spiInstance->transferCompleteCallback(hspi);
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    // if we have multiple SPI ports, then maybe do a lookup of which c++ instance to call
    _spiInstance->transferHalfCompleteCallback(hspi);

}

/**
 * @brief  SPI error callbacks.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{

}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
