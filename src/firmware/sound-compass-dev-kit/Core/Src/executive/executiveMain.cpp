//-------------------------------------------------------------------
// Module       : executiveMain.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

#include "string.h"
#include "cmsis_os.h"
#include <stdio.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
//#include "etl/queue.h"
//#include "etl/fsm.h"
//#include "etl/message_packet.h"

#include "executive/executiveMain.hpp"
#include "board/interfaces/IBoardHardware.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char* TAG = "Exec";

#define DIAG_LINE_INFO(diag, tag, message, ...) diag->info_line(TAG, __BASE_FILE__ , __LINE__,  message,  __VA_ARGS__ );
//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------


bool executive::executiveMain::initialise(board::IBoardHardware *hardware)
{
    _hardware = hardware;



    return true;
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;
    while (1)
    {
        _hardware->LED_debugGreen->toggleLED();
        _hardware->diag->info(TAG,  " Toggle at %d\n", __LINE__ );
        DIAG_LINE_INFO(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        printf("printf: %d %d\n", _count++, __LINE__);
        fflush(stdout);

        osDelay(100);

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

