//-------------------------------------------------------------------
// Module       : executiveMain.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

#include "string.h"
#include "cmsis_os.h"

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

    while (1)
    {
        _hardware->LED_debugGreen->toggleLED();
//        (*(*_hardware).LED_debugGreen).toggleLED();
        osDelay(100);

    }
}

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

