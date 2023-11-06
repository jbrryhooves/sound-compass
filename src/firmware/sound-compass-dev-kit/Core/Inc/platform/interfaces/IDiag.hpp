/*
 * IDiag.hpp
 *
 *  Created on: Oct 19, 2023
 *      Author: Josh
 */

#ifndef INC_PLATFORM_INTERFACES_IDIAG_HPP_
#define INC_PLATFORM_INTERFACES_IDIAG_HPP_
//-------------------------------------------------------------------
// Module       : IDiag.hpp
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
#define DIAG_LINE_ERROR(diag, tag, message, ...) diag->error_line(tag, __BASE_FILE__ , __LINE__,  message,  __VA_ARGS__ );
#define DIAG_LINE_WARN(diag, tag, message, ...) diag->warn_line(tag, __BASE_FILE__ , __LINE__,  message,  __VA_ARGS__ );
#define DIAG_LINE_INFO(diag,tag,message,...) (diag)->info_line((tag), __BASE_FILE__ , __LINE__,  (message),  __VA_ARGS__ );
#define DIAG_LINE_DEBUG(diag, tag, message, ...) diag->debug_line(tag, __BASE_FILE__ , __LINE__,  message,  __VA_ARGS__ );
#define DIAG_LINE_VERBOSE(diag, tag, message, ...) diag->verbose_line(tag, __BASE_FILE__ , __LINE__,  message,  __VA_ARGS__ );

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace platform
{

    class IDiag
    {
    public:

        enum class Level
        {
            // Ordered critical to non-critical
            ERROR,
            WARN,
            INFO,
            DEBUG_log,
            VERBOSE
        };

        /**
         * @brief set the current debugging level for the tag
         *
         * @param tag TAG for debug
         * @param level Debug level
         */
        virtual void setLevel(const char *tag, Level level) = 0;

        virtual void atLevel(IDiag::Level level, const char *tag, const char *format, ...) = 0;
        virtual void error(const char *tag, const char *format, ...) = 0;
        virtual void warn(const char *tag, const char *format, ...) = 0;
        virtual void info(const char *tag, const char *format, ...) = 0;
        virtual void debug(const char *tag, const char *format, ...) = 0;
        virtual void verbose(const char *tag, const char *format, ...) = 0;

        virtual void error_line(const char *tag, const char *file, uint16_t line, const char *format, ...) = 0;
        virtual void warn_line(const char *tag, const char *file, uint16_t line, const char *format, ...) = 0;
        virtual void info_line(const char *tag, const char *file, uint16_t line, const char *format, ...) = 0;
        virtual void debug_line(const char *tag, const char *file, uint16_t line, const char *format, ...) = 0;
        virtual void verbose_line(const char *tag, const char *file, uint16_t line, const char *format, ...) = 0;

        /**
         * @brief Flush out-going debug
         *
         */
        virtual void flush(void) = 0;
    };
}

#endif /* INC_PLATFORM_INTERFACES_IDIAG_HPP_ */

