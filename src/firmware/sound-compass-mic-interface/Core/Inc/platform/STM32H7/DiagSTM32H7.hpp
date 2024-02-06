/*
 * DiagSTM32H7.hpp
 *
 *  Created on: Oct 19, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_STM32H7_DIAGSTM32H7_HPP_
#define INC_PLATFORM_STM32H7_DIAGSTM32H7_HPP_
//-------------------------------------------------------------------
// Module       : DiagSTM32H7.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "platform/interfaces/IDiag.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platformSTM32
{
    /*!
     \brief Controller hardware interface
     */
    class DiagSTM32: public platform::IDiag
    {
    public:
        static const unsigned int LOG_EXTRAS = 180;
        static const unsigned int LOG_LENGTH = 220;

        /**
         * @brief Initialise the diagnostics
         *
         * @return true success
         * @return false
         */
        bool initialise(UART_HandleTypeDef *_uartHandle);

        // IDiag
        void setLevel(const char *tag, IDiag::Level level);
        void atLevel(IDiag::Level level, const char *tag, const char *format, ...);
        void error(const char *tag, const char *format, ...);
        void warn(const char *tag, const char *format, ...);
        void info(const char *tag, const char *format, ...);
        void debug(const char *tag, const char *format, ...);
        void verbose(const char *tag, const char *format, ...);

        void error_line(const char *tag, const char *file, uint16_t line, const char *format, ...);
        void warn_line(const char *tag, const char *file, uint16_t line, const char *format, ...);
        void info_line(const char *tag, const char *file, uint16_t line, const char *format, ...);
        void debug_line(const char *tag, const char *file, uint16_t line, const char *format, ...);
        void verbose_line(const char *tag, const char *file, uint16_t line, const char *format, ...);

        void flush(void);

    private:

        UART_HandleTypeDef *_uartHandle;
        IDiag::Level _logLevel = IDiag::Level::DEBUG_log;
    };
}

#endif /* INC_PLATFORM_STM32H7_DIAGSTM32H7_HPP_ */

