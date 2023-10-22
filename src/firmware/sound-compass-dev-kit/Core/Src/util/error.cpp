/*
 * error.c
 *
 */

#include <stdio.h>
#include <util/error.hpp>

void error_fatal(eFatalSystemError fse, const char *file, uint32_t line)
{
    static char errStr[255];

    // Attempt to send distress signal by any means possible
    sprintf(errStr, "\nFatal system error: %d (%s:%lu)\n", fse, file, line);
    printf(errStr);

    __asm("BKPT #0\n");
    // break into the debugger so we can try figure out what happened

    // Then just halt and wait for the watchdog
    while (1)
        ;
}
