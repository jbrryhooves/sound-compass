//-------------------------------------------------------------------
// Module       : Diag.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <platform/STM32_HAL/DiagSTM32H7.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <util/assert.hpp>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

// log colours copied from https://xdevs.com/guide/color_serial/
#define LOG_COLOR_E     "\033[1;31m"
#define LOG_COLOR_W     "\033[1;33m"
#define LOG_COLOR_I     "\033[1;32m"
#define LOG_COLOR_D     "\033[1;37m"
#define LOG_COLOR_V     "\033[0;37m"
#define LOG_RESET_COLOR "\033[1;39m"

static void logPrint(platform::IDiag::Level level, const char *tag, const char *prefix, const char *format, const char *fileName, const uint16_t fileLine, va_list args);
static UART_HandleTypeDef *_printf_uartHandle;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platform::stm32hal::DiagSTM32::initialise(UART_HandleTypeDef *uartHandle)
{
    _uartHandle = uartHandle;
    _printf_uartHandle = uartHandle;

    // disable buffering of stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    return true;

}


static void logPrint(platform::IDiag::Level level, const char *tag, const char *prefix, const char *format, const char *fileName, const uint16_t fileLine, va_list args)
{
    static char logBuffer[platform::stm32hal::DiagSTM32::LOG_LENGTH + platform::stm32hal::DiagSTM32::LOG_EXTRAS + 1];
    int res = 0;
    unsigned int n = 0;

    uint32_t _ticks = HAL_GetTick();
    uint32_t _seconds = _ticks / 1000 % 60;
    uint32_t _min = _ticks / (1000 * 60) % 60;
    uint32_t _hr = _ticks / (1000 * 3600);

    res = snprintf(logBuffer, platform::stm32hal::DiagSTM32::LOG_EXTRAS, "%s (%02lu:%02lu:%02lu.%03lu ) %s: ", prefix, _hr, _min, _seconds, _ticks % 1000, tag);

    if (res <= 0)
    {
        logBuffer[platform::stm32hal::DiagSTM32::LOG_EXTRAS] = 0;

    }
    else
    {
        n += res;
    }

    if (fileLine > 0)
    {
        res = snprintf(&logBuffer[n], platform::stm32hal::DiagSTM32::LOG_EXTRAS - n, "%s:%d: ", fileName, fileLine);

        if (res <= 0)
        {
            logBuffer[platform::stm32hal::DiagSTM32::LOG_EXTRAS - 1] = 0;
        }
        else
        {
            n += res;
        }
    }

    res = vsnprintf(&logBuffer[n], platform::stm32hal::DiagSTM32::LOG_LENGTH, format, args);
    if (res <= 0)
    {
        logBuffer[n + platform::stm32hal::DiagSTM32::LOG_LENGTH - 1] = 0;

    }
    else
    {
        n += res;
    }

    res = snprintf(&logBuffer[n], platform::stm32hal::DiagSTM32::LOG_EXTRAS - n, "%s",
    LOG_RESET_COLOR);
    if (res <= 0)
    {
        logBuffer[platform::stm32hal::DiagSTM32::LOG_EXTRAS - 1] = 0;

    }
    else
    {
        n += res;
    }

    logBuffer[platform::stm32hal::DiagSTM32::LOG_LENGTH + platform::stm32hal::DiagSTM32::LOG_EXTRAS] = 0;

//    sprintf("%d %s %s %c \n", level, tag,(const char*)logBuffer, LOG_RESET_COLOR);
    HAL_StatusTypeDef err = HAL_UART_Transmit(_printf_uartHandle, (const uint8_t*) logBuffer, n, 0xFFFF);
    ASSERT(err == HAL_OK);
    return;
}

//-------------------------------------------------------------------
// Public - IDiag
//-------------------------------------------------------------------
void platform::stm32hal::DiagSTM32::setLevel(const char *tag, IDiag::Level level)
{
    _logLevel = level;

    // TODO: create a etl::map<tag, logLevel> and populate that.
}

void platform::stm32hal::DiagSTM32::atLevel(IDiag::Level level, const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    switch (level)
    {
        default:
        case IDiag::Level::ERROR:
            logPrint(IDiag::Level::ERROR, tag, LOG_COLOR_E "E", format, "", 0, args);
            break;
        case IDiag::Level::WARN:
            logPrint(IDiag::Level::WARN, tag, LOG_COLOR_W "W", format, "", 0, args);
            break;
        case IDiag::Level::INFO:
            logPrint(IDiag::Level::INFO, tag, LOG_COLOR_I "I", format, "", 0, args);
            break;
        case IDiag::Level::DEBUG_log:
            logPrint(IDiag::Level::DEBUG_log, tag, LOG_COLOR_D "D", format, "", 0, args);
            break;
        case IDiag::Level::VERBOSE:
            logPrint(IDiag::Level::VERBOSE, tag, LOG_COLOR_V "V", format, "", 0, args);
            break;
    }
    va_end(args);
}

void platform::stm32hal::DiagSTM32::error(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::ERROR, tag, LOG_COLOR_E "E", format, "", 0, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::warn(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::WARN, tag, LOG_COLOR_W "W", format, "", 0, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::info(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::INFO, tag, LOG_COLOR_I "I", format, "", 0, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::debug(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::DEBUG_log, tag, LOG_COLOR_D "D", format, "", 0, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::verbose(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::VERBOSE, tag, LOG_COLOR_V "V", format, "", 0, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::error_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::ERROR, tag, LOG_COLOR_E "E", format, file, line, args);

    va_end(args);
}
void platform::stm32hal::DiagSTM32::warn_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::WARN, tag, LOG_COLOR_W "W", format, file, line, args);

    va_end(args);
}
void platform::stm32hal::DiagSTM32::info_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::INFO, tag, LOG_COLOR_I "I", format, file, line, args);

    va_end(args);
}
void platform::stm32hal::DiagSTM32::debug_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::DEBUG_log, tag, LOG_COLOR_D "D", format, file, line, args);

    va_end(args);
}
void platform::stm32hal::DiagSTM32::verbose_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::VERBOSE, tag, LOG_COLOR_V "V", format, file, line, args);

    va_end(args);
}

void platform::stm32hal::DiagSTM32::flush(void)
{
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

extern "C"
{
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART1 and Loop until the end of transmission */
    if (_printf_uartHandle != nullptr)
    {
        HAL_UART_Transmit(_printf_uartHandle, (uint8_t*) &ch, 1, 0xFFFF);

    }

    return ch;
}

}
