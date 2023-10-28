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
#include "etl/hfsm.h"
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
    _stateMachine.SetStates();

    _stateMachine.start();

    return true;
}

bool executive::executiveMain::run()
{
    uint8_t _count = 0;
    while (1)
    {

        _hardware->LED_debugGreen->toggleLED();
        _hardware->diag->info(TAG,  "Toggle at %d\n", __LINE__ );
        DIAG_LINE_ERROR(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_INFO(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_WARN(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_DEBUG(_hardware->diag, TAG,  "Toggle %d\n", _count++);
        DIAG_LINE_VERBOSE(_hardware->diag, TAG,  "Toggle %d\n", _count++);


        _stateMachine.receive(HeartBeatMessage());

        osDelay(100);

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

