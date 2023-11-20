//-------------------------------------------------------------------
// Module       : audioProcessor.cpp
// Description  : 
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// System Includes
//-------------------------------------------------------------------
extern "C"
{

#include "cmsis_os.h"
#include "arm_math.h"
}
#include "main.h"
#include "stdio.h"

//-------------------------------------------------------------------
// Local Includes
//-------------------------------------------------------------------

#include "executive/audioProcessor.hpp"

//-------------------------------------------------------------------
// Definitions
//-------------------------------------------------------------------
static const char *TAG = "audioProc";

osThreadId_t audioProcessorTaskHandle;
uint32_t audioProcessorTaskBuffer[1024 * 4];
StaticTask_t audioProcessorTaskControlBlock;
const osThreadAttr_t audioProcessorTask_attributes = {
        .name = "audioProcessorTask",
        .cb_mem = &audioProcessorTaskControlBlock,
        .cb_size = sizeof(audioProcessorTaskControlBlock),
        .stack_mem = &audioProcessorTaskBuffer[0],
        .stack_size = sizeof(audioProcessorTaskBuffer),
        .priority = (osPriority_t) osPriorityAboveNormal, };

//-------------------------------------------------------------------
// Public
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Private
//-------------------------------------------------------------------

#define IIR_ORDER     2
#define IIR_NUMSTAGES (IIR_ORDER/2)

static float32_t m_biquad_state[IIR_ORDER] __attribute__ ((section (".my_data")));
static float32_t m_biquad_coeffs[5 * IIR_NUMSTAGES] __attribute__ ((section (".my_data"))) = {
        0.99799,
        -1.99597,
        0.99799,
        1.99597,
        -0.99598 };

arm_biquad_cascade_df2T_instance_f32 const iir_inst = {
        IIR_ORDER / 2,
        m_biquad_state,
        m_biquad_coeffs };

const static uint16_t BLOCK_SIZE = 960; // 33 1/3rd octave bands with 480 samples.
                                        // 480 samples @ 48kHz = 10ms
static float32_t pSrc[BLOCK_SIZE] __attribute__ ((section (".my_data"))) = { 0.8 };
static float32_t pDst[BLOCK_SIZE] __attribute__ ((section (".my_data"))) = { 0.0 };



bool executive::audioProcessor::initialise(board::IBoardHardware *hardware, IAudioProcessorListener *audioProcessedListener, platform::ITaskFactory *taskFactory)
{
    _hardware = hardware;
    _audioProcessedListener = audioProcessedListener;
    _taskFactory = taskFactory;

    return true;
}

bool executive::audioProcessor::start()
{
    _hardware->diag->info(TAG, "Starting..\n");
    _taskHandle = _taskFactory->createTaskStatic("audioProcessorTask", &audioProcessorTaskControlBlock, sizeof(audioProcessorTaskControlBlock), &audioProcessorTaskBuffer[0],
            sizeof(audioProcessorTaskBuffer), osPriorityAboveNormal, this /* ITask*/);

    if (_taskHandle == nullptr)
    {
        _hardware->diag->error(TAG, "Can not create audioProcessorTask task");
        return false;
    }
    return true;
}

// ITask
void executive::audioProcessor::taskMain(void)
{


    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        pSrc[i] = 0.9;
    }
    
    uint8_t filterCount = 10;
    uint16_t count = 0;
    uint32_t start, stop, diff;
    while (1)
    {
//        printf("audio processor\n");

        _audioProcessedListener->onAudioFrameProcessed();
        start = HAL_GetTick();
        for (int i = 0; i < filterCount; ++i) // 33 octave bands
        {
            arm_biquad_cascade_df2T_f32(&iir_inst, pSrc, pDst, BLOCK_SIZE);
        }
        stop = HAL_GetTick();

        diff = stop - start;

        ProcessingMetrics_t _metrics = { .fullProcessingTime_ms = diff, .filterCount = filterCount };
        _audioProcessedListener->onAudioFrameMetrics(&_metrics);
        osDelay(700);

        count++;
        if (count % 10 == 0)
        {
            filterCount += 10;
        }
    }
}

bool executive::audioProcessor::killEnable(bool enable)
{

}
