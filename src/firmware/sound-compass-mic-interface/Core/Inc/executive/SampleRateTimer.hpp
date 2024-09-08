/*
 * ADC_AD7768.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_SampleRateTimer_HPP_
#define INC_EXECUTIVE_SampleRateTimer_HPP_
//-------------------------------------------------------------------
// Module       : ADC_AD7768.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
#include <array>
#include "stdint.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "board/interfaces/IBoardHardware.hpp"
#include "platform/interfaces/IHardwareTimer.hpp"
#include "platform/interfaces/IHardwarePWM.hpp"


//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class SampleRateTimer: public platform::IHardwareTimer, public platform::IHardwarePWM
    {
    public:

        struct Config
        {
            platform::IHardwareTimer *sampleRateTimer;
            uint8_t channel;
            uint32_t frequency_hz;
            float duty_percent;
            
        };

        enum class ADCError
        {
            Ok,
        };

        bool initialise(board::IBoardHardware *hardware, Config config);
        bool configure();
        bool start();


        // IHardwareTimer
        bool stop(void);
        bool start(unsigned int ms);
        bool reset(void);
        bool setFreq(unsigned int freq_hz);

        // IHardwarePWM
        bool stopPWM(void);
        bool startPWM(unsigned int ms);
        bool setDuty(float percent);

    private:

        board::IBoardHardware *_hardware;
        Config _config;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_ADC_AD7768_HPP_ */

