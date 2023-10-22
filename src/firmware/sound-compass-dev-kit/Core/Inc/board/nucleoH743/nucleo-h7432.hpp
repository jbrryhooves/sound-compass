/*
 * nucleo-h7432.hpp
 *
 *  Created on: Sep 20, 2023
 *      Author: Josh
 */

#ifndef INC_BOARD_NUCLEO_H7432_HPP_
#define INC_BOARD_NUCLEO_H7432_HPP_
//-------------------------------------------------------------------
// Module       : nucleo-h7432.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "board/interfaces/IBoardHardware.hpp"
#include "board/nucleoH743/LED.hpp"
#include "board/nucleoH743/spi.hpp"

#include "platform/STM32H7/DiagSTM32H7.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace board
{
    class boardNucleoH743: public board::IBoardHardware
    {
    public:
        bool initialise();


    private:
        platformSTM32::DiagSTM32 _diagSTM32;
        board::nucleoH743::LED _LED_debugGreen;
        board::nucleoH743::SPI _spi;
    };
}

#endif /* INC_BOARD_NUCLEO_H7432_HPP_ */

