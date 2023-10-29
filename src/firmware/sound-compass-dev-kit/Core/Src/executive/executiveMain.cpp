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
//#include "etl/message_packet.h"

#include "executive/executiveMain.hpp"
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/IDiag.hpp"


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char* TAG = "Exec";



//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------


bool executive::executiveMain::initialise(board::IBoardHardware *hardware)
{
    _hardware = hardware;

    _audioProcessor.initialise(_hardware, _hardware->taskFactory);

    return true;
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;

    _audioProcessor.start();

    while (1)
    {

        _hardware->LED_debugGreen->toggleLED();
//        _hardware->diag->info(TAG,  "Toggle at %d\n", __LINE__ );
//        DIAG_LINE_ERROR(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_INFO(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_WARN(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_DEBUG(_hardware->diag, TAG,  "Toggle %d\n", _count++);
//        DIAG_LINE_VERBOSE(_hardware->diag, TAG,  "Toggle %d\n", _count++);



        osDelay(500);

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

