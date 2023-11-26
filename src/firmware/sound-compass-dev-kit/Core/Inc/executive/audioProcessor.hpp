/*
 * audioProcessor.hpp
 *
 *  Created on: Oct 28, 2023
 *      Author: Josh
 */

#ifndef INC_EXECUTIVE_AUDIOPROCESSOR_HPP_
#define INC_EXECUTIVE_AUDIOPROCESSOR_HPP_
//-------------------------------------------------------------------
// Module       : audioProcessor.hpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------
#include "platform/interfaces/ITask.hpp"
#include "board/interfaces/IBoardHardware.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

namespace executive
{

    class audioProcessor: public platform::ITask
    {
    public:

        // Number of IEC 61260-1-2014 1/3 octave filter bands covering band 14 (25Hz centre freq) to band 43 (20kHz centre freq)
        const static uint8_t FILTER_BANDS = 30;

        typedef struct _ProcessingMetrics
        {
            uint32_t fullProcessingTime_ms;
            uint32_t timestamp;
            // The steered angle of the radial filter set
            float steeringAngle;
            uint8_t filterCount;
            // the filter results for this steered angle
            float radialFilterSet[FILTER_BANDS];
            // the filter results for the centre mic
            float centreFilterSet[FILTER_BANDS];

        } ProcessingMetrics_t;




        class IAudioProcessorListener
        {
        public:
            virtual void onAudioFrameProcessed(void) = 0;
            virtual void onAudioFrameMetrics(const ProcessingMetrics_t *metrics) = 0;
        };

        bool initialise(board::IBoardHardware *hardware, IAudioProcessorListener *audioProcessedListener, platform::ITaskFactory *taskFactory);
        bool start();

        // ITask
        void taskMain(void);
        bool killEnable(bool enable);

    private:

//        platform::ITimer *_debugTimer;
        board::IBoardHardware *_hardware;
        platform::ITaskFactory *_taskFactory;
        platform::ITaskFactory::TaskHandle _taskHandle;
        IAudioProcessorListener *_audioProcessedListener;

    };

} /* namespace executive */

#endif /* INC_EXECUTIVE_AUDIOPROCESSOR_HPP_ */

