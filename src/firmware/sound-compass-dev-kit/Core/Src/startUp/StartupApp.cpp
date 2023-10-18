//-------------------------------------------------------------------
// Module       : App
// Description  : Application C++

//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <app/StartupApp.hpp>
#include <stdio.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "executive/executiveMain.hpp"

#include "board/nucleoH743/nucleo-h7432.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------
#include <app/StartupApp.hpp>


static startup::StartupApp _startupApp;
extern "C" void executiveStart_c(void)
{
    // call into C++
    _startupApp.mainLoop();
}

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------
void startup::StartupApp::mainLoop(void)
{

    // choose which application to run
    executive::executiveMain _executive;

    // choose what hardware we're running on
    board::boardNucleoH743 _nucleoBoard;

    if(!_nucleoBoard.initialise())
    {
        while (1);
    }

    // Initialise and run application
    board::IBoardHardware* _hw = &_nucleoBoard;
    if (!_executive.initialise(_hw))
    {
        while (1)
            ;
        return;
    }

    if (!_executive.run())
    {
        while (1)
            ;
        return;
    }
}
