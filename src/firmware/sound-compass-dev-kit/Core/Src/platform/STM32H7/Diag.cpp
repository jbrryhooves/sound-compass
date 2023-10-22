
//-------------------------------------------------------------------
// Module       : Diag.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <util/assert.hpp>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "main.h"
#include "platform/STM32H7/DiagSTM32H7.hpp"

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

#define LOG_COLOR_E ""
#define LOG_COLOR_W ""
#define LOG_COLOR_I ""
#define LOG_COLOR_D ""
#define LOG_COLOR_V ""
#define LOG_RESET_COLOR ""

static void logPrint(platform::IDiag::Level level, const char* tag, const char* prefix, const char* format, const char* fileName, const uint16_t fileLine, va_list args);
static UART_HandleTypeDef* _printf_uartHandle;
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

bool platformSTM32::DiagSTM32::initialise(UART_HandleTypeDef* uartHandle)
{
    _uartHandle = uartHandle;
    _printf_uartHandle = uartHandle;

    setvbuf(stdout, NULL, _IONBF, 0);

    return true;

}

// IDiag
static void logPrint(platform::IDiag::Level level, const char* tag, const char* prefix, const char* format, const char* fileName, const uint16_t fileLine, va_list args)
{
    static char logBuffer[platformSTM32::DiagSTM32::LOG_LENGTH + platformSTM32::DiagSTM32::LOG_EXTRAS + 1];
    unsigned int n = 0;

    uint32_t _ticks = HAL_GetTick();
    uint32_t _seconds = _ticks/1000 % 60;
    uint32_t _min = _ticks/(1000 * 60) % 60;
    uint32_t _hr = _ticks/(1000 * 3600);


    n += snprintf(logBuffer,
                platformSTM32::DiagSTM32::LOG_EXTRAS - n,
              "%s (%02lu:%02lu:%02lu.%03lu ) %s: ",
              prefix,
              _hr,
              _min,
              _seconds,
              _ticks % 1000,
              tag);

    if(fileLine > 0)
    {
        n += snprintf(&logBuffer[n],
                    platformSTM32::DiagSTM32::LOG_EXTRAS - n,
                  "%s:%d: ",
                  fileName,
                  fileLine);

    }

    n += vsnprintf(&logBuffer[n], platformSTM32::DiagSTM32::LOG_LENGTH, format, args);
    logBuffer[platformSTM32::DiagSTM32::LOG_LENGTH + platformSTM32::DiagSTM32::LOG_EXTRAS] = 0;

    printf("dfdf\n");
//    sprintf("%d %s %s %c \n", level, tag,(const char*)logBuffer, LOG_RESET_COLOR);
    HAL_StatusTypeDef res = HAL_UART_Transmit(_printf_uartHandle, (const uint8_t*)logBuffer, n, 0xFFFF);
    ASSERT(res == HAL_OK);
    return;
}

//-------------------------------------------------------------------
// Public - IDiag
//-------------------------------------------------------------------
void platformSTM32::DiagSTM32::setLevel(const char* tag, IDiag::Level level)
{
    _logLevel = level;

    // TODO: create a etl::map<tag, logLevel> and populate that.
}

void platformSTM32::DiagSTM32::atLevel(IDiag::Level level, const char* tag, const char* format, ...)
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

void platformSTM32::DiagSTM32::error(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::ERROR, tag, LOG_COLOR_E "E", format, "", 0, args);

    va_end(args);
}

void platformSTM32::DiagSTM32::warn(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::WARN, tag, LOG_COLOR_W "W", format, "", 0, args);

    va_end(args);
}

void platformSTM32::DiagSTM32::info(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::INFO, tag, LOG_COLOR_I "I", format, "", 0, args);

    va_end(args);
}

void platformSTM32::DiagSTM32::debug(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::DEBUG_log, tag, LOG_COLOR_D "D", format, "", 0, args);

    va_end(args);
}

void platformSTM32::DiagSTM32::verbose(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::VERBOSE, tag, LOG_COLOR_V "V", format, "", 0, args);

    va_end(args);
}


void platformSTM32::DiagSTM32::info_line(const char *tag, const char *file, uint16_t line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    logPrint(IDiag::Level::INFO, tag, LOG_COLOR_I "I", format, file, line, args);

    va_end(args);
}

void platformSTM32::DiagSTM32::flush(void)
{
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART1 and Loop until the end of transmission */
    if(_printf_uartHandle != nullptr)
    {
        HAL_UART_Transmit(_printf_uartHandle, (uint8_t *)&ch, 1, 0xFFFF);

    }

    return ch;
}

int _write(int file, char *ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        __io_putchar (*ptr++);
    }
    return len;
}
