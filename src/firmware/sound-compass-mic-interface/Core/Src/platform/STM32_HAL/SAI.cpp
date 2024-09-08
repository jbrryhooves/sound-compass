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
#include <platform/STM32_HAL/SAI.hpp>
#include <platform/STM32_HAL/GPIO.hpp>
#include "util/assert.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

static platform::stm32hal::SAI *_saiInstance = nullptr;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platform::stm32hal::SAI::initialise(SAI_HandleTypeDef *spiHandleA, SAI_HandleTypeDef *spiHandleB, DMA_HandleTypeDef *spiDMAhandleRxA,DMA_HandleTypeDef *spiDMAhandleRxB)
{
    // for now, until we have several SPI ports, this SPI instance must be a singleton. If another instance has been created, then we need to do something different
    ASSERT(_saiInstance == nullptr);
    _saiInstance = this;

    _saiHandleA = spiHandleA;
    _saiDMAhandleRxA = spiDMAhandleRxA;
    _saiHandleB = spiHandleB;
    _saiDMAhandleRxB = spiDMAhandleRxB;

    return true;
}

void platform::stm32hal::SAI::transferHalfCompleteCallback(SAI_HandleTypeDef *hspi)
{

}

void platform::stm32hal::SAI::transferCompleteCallback(SAI_HandleTypeDef *hsai)
{
    if (_saiInstance->_callback != nullptr)
    {
        uint8_t saiChannel = hsai == _saiHandleA ? 0 : 1;
        _saiInstance->_callback(saiChannel);
    }
}

//-------------------------------------------------------------------
// Public ISPI
//-------------------------------------------------------------------
void platform::stm32hal::SAI::registerCallback(platform::ISAI::bytesReceivedCallback_t *callback)
{
    _callback = callback;
}


bool platform::stm32hal::SAI::receiveDMA(uint8_t *pRxData_A, uint8_t *pRxData_B, uint16_t size)
{
    auto retA = HAL_SAI_Receive_DMA(_saiHandleA, pRxData_A, size);
    auto retB = HAL_SAI_Receive_DMA(_saiHandleB, pRxData_B, size);
    
    return retA == HAL_OK && retB == HAL_OK;
}

// void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai);
// void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
// void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
// void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
// void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
// void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);

//-------------------------------------------------------------------
// STM32 HAL C Callbacks
//-------------------------------------------------------------------


/**
 * @brief  Rx Transfer completed callback.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report end of DMA TxRx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
extern "C"
{
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
    {
    if (_saiInstance != nullptr)
    {
        _saiInstance->transferCompleteCallback(hsai);
    }
    }
}
//void HAL_SAI_TxRxHalfCpltCallback(SAI_HandleTypeDef *hsai)
//{
//    if(_saiInstance != nullptr)
//    {
//        // if we have multiple SPI ports, then maybe do a lookup of which c++ instance to call
//        _saiInstance->transferHalfCompleteCallback(hsai);
//    }
//}

/**
 * @brief  SPI error callbacks.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
extern "C"
{
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    uint32_t errCode = hsai->ErrorCode;
}
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
