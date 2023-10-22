/*
 * IBoardHardware.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_
#define INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <stdbool.h>

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/IDiag.hpp"
#include "platform/interfaces/ILed.hpp"
#include "platform/interfaces/ISPI.hpp"
//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace board
{
    class IBoardHardware
    {
    public:
        platform::IDiag* diag;
        platform::ILed* LED_debugGreen;
        platform::ISPI* spi;

    };
}

#endif /* INC_BOARD_INTERFACES_IBOARDHARDWARE_HPP_ */
