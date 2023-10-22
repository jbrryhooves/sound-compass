/**
 * \file
 *
 * \brief
 *
 * \author Josh Berry
 * \date
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdint.h>

//=====================
//      DEFINES
//=====================
// Make calling error_fatal() easier
#define ERROR_FATAL(fse)  error_fatal(fse, __FILE__, __LINE__)

//=========
//  Types
//=========

typedef enum FatalSystemError {
    fseUnknown = 0,

    fseAssert = 1,    //An assert has failed

    fseMAX
} eFatalSystemError;

//=============
//  Interface
//=============

// Call if a fatal error is detected that warrants halting and watchdog reboot
// Use ERROR_FATAL instead of this function directly to handle file/line params.
void error_fatal(eFatalSystemError fse, const char *file, uint32_t line);

#endif //ERROR_H_
