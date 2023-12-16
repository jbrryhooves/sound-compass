#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <numbers>

#include <algorithm>
#include <chrono>
#include <vector>
#include <mutex>


#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <kfr/base.hpp>
#include <kfr/dsp/biquad.hpp>
#include <kfr/dsp.hpp>
#include <kfr/io.hpp>


#include <audioProcessor/beamProcessor.hpp>
#include <interfaces/IProcessedAudioResultListener.hpp>

using namespace std::chrono;


const static float_t SAMPLE_RATE_HZ = 48000.0;

/// <summary>
/// The filter state for a particular octave band
/// </summary>
typedef struct
{
    int filterBandNumber;
    float lowerFreqCutOff;
    float centerFreq;
    float upperFreqCutOff;
    std::vector<kfr::biquad_params<float_t>> secondOrderStagesCoefficients;
    std::unique_ptr<kfr::biquad_filter<float_t>> biquadFilter;
    std::mutex filterMutex;

} OctaveFilter;

typedef struct _BeamFilterState
{
    std::array<OctaveFilter, audioProcessor::beamProcessor::FILTER_COUNT> thirdOctaveFilters;
    

} BeamFilterState;


// clang-format off
static std::array<OctaveFilter, audioProcessor::beamProcessor::FILTER_COUNT> centerMicThirdOctaveFilters = {
    OctaveFilter{ .filterBandNumber = 14, .lowerFreqCutOff = 22.38721139,	.centerFreq =25.11886432, 	.upperFreqCutOff =28.18382931},
    OctaveFilter{ .filterBandNumber = 15, .lowerFreqCutOff = 28.18382931,	.centerFreq =31.6227766, 	.upperFreqCutOff =35.48133892},
    OctaveFilter{ .filterBandNumber = 16, .lowerFreqCutOff = 35.48133892,	.centerFreq =39.81071706, 	.upperFreqCutOff =44.66835922},
    OctaveFilter{ .filterBandNumber = 17, .lowerFreqCutOff = 44.66835922,	.centerFreq =50.11872336, 	.upperFreqCutOff =56.23413252},
    OctaveFilter{ .filterBandNumber = 18, .lowerFreqCutOff = 56.23413252,	.centerFreq =63.09573445, 	.upperFreqCutOff =70.79457844},
    OctaveFilter{ .filterBandNumber = 19, .lowerFreqCutOff = 70.79457844,	.centerFreq =79.43282347, 	.upperFreqCutOff =89.12509381},
    OctaveFilter{ .filterBandNumber = 20, .lowerFreqCutOff = 89.12509381,	.centerFreq =100.0, 	    .upperFreqCutOff =112.2018454},
    OctaveFilter{ .filterBandNumber = 21, .lowerFreqCutOff = 112.2018454,	.centerFreq =125.8925412, 	.upperFreqCutOff =141.2537545},
    OctaveFilter{ .filterBandNumber = 22, .lowerFreqCutOff = 141.2537545,	.centerFreq =158.4893192, 	.upperFreqCutOff =177.827941},
    OctaveFilter{ .filterBandNumber = 23, .lowerFreqCutOff = 177.827941,	.centerFreq =199.5262315, 	.upperFreqCutOff =223.8721139},
    OctaveFilter{ .filterBandNumber = 24, .lowerFreqCutOff = 223.8721139,	.centerFreq =251.1886432, 	.upperFreqCutOff =281.8382931},
    OctaveFilter{ .filterBandNumber = 25, .lowerFreqCutOff = 281.8382931,	.centerFreq =316.227766, 	.upperFreqCutOff =354.8133892},
    OctaveFilter{ .filterBandNumber = 26, .lowerFreqCutOff = 354.8133892,	.centerFreq =398.1071706, 	.upperFreqCutOff =446.6835922},
    OctaveFilter{ .filterBandNumber = 27, .lowerFreqCutOff = 446.6835922,	.centerFreq =501.1872336, 	.upperFreqCutOff =562.3413252},
    OctaveFilter{ .filterBandNumber = 28, .lowerFreqCutOff = 562.3413252,	.centerFreq =630.9573445, 	.upperFreqCutOff =707.9457844},
    OctaveFilter{ .filterBandNumber = 29, .lowerFreqCutOff = 707.9457844,	.centerFreq =794.3282347, 	.upperFreqCutOff =891.2509381},
    OctaveFilter{ .filterBandNumber = 30, .lowerFreqCutOff = 891.2509381,	.centerFreq =1000.0, 	    .upperFreqCutOff =1122.018454},
    OctaveFilter{ .filterBandNumber = 31, .lowerFreqCutOff = 1122.018454,	.centerFreq =1258.925412, 	.upperFreqCutOff =1412.537545},
    OctaveFilter{ .filterBandNumber = 32, .lowerFreqCutOff = 1412.537545,	.centerFreq =1584.893192, 	.upperFreqCutOff =1778.27941},
    OctaveFilter{ .filterBandNumber = 33, .lowerFreqCutOff = 1778.27941,	.centerFreq =1995.262315, 	.upperFreqCutOff =2238.721139},
    OctaveFilter{ .filterBandNumber = 34, .lowerFreqCutOff = 2238.721139,	.centerFreq =2511.886432, 	.upperFreqCutOff =2818.382931},
    OctaveFilter{ .filterBandNumber = 35, .lowerFreqCutOff = 2818.382931,	.centerFreq =3162.27766, 	.upperFreqCutOff =3548.133892},
    OctaveFilter{ .filterBandNumber = 36, .lowerFreqCutOff = 3548.133892,	.centerFreq =3981.071706, 	.upperFreqCutOff =4466.835922},
    OctaveFilter{ .filterBandNumber = 37, .lowerFreqCutOff = 4466.835922,	.centerFreq =5011.872336, 	.upperFreqCutOff =5623.413252},
    OctaveFilter{ .filterBandNumber = 38, .lowerFreqCutOff = 5623.413252,	.centerFreq =6309.573445, 	.upperFreqCutOff =7079.457844},
    OctaveFilter{ .filterBandNumber = 39, .lowerFreqCutOff = 7079.457844,	.centerFreq =7943.282347, 	.upperFreqCutOff =8912.509381},
    OctaveFilter{ .filterBandNumber = 40, .lowerFreqCutOff = 8912.509381,	.centerFreq =10000.0, 	    .upperFreqCutOff =11220.18454},
    OctaveFilter{ .filterBandNumber = 41, .lowerFreqCutOff = 11220.18454,	.centerFreq =12589.25412, 	.upperFreqCutOff =14125.37545},
    OctaveFilter{ .filterBandNumber = 42, .lowerFreqCutOff = 14125.37545,	.centerFreq =15848.93192, 	.upperFreqCutOff =17782.7941},
    OctaveFilter{ .filterBandNumber = 43, .lowerFreqCutOff = 17782.7941,	.centerFreq =19952.62315, 	.upperFreqCutOff =22387.21139},
};

// clang-format on

// this is an array instead of a vector because the OctaveFilter struct contains a mutex, which isn't copyable, and a vector needs it's elements to be copyable. 
static std::array<BeamFilterState, audioProcessor::beamProcessor::BEAM_COUNT> _beamFilterStates;



//typedef struct 
//{
//    int filterBandNumber;
//    float lowerFreqCutOff;
//    float centerFreq;
//    float upperFreqCutOff;
//    std::vector<kfr::biquad_params<float_t>> secondOrderStagesCoefficients;
//    std::unique_ptr<kfr::biquad_filter<float_t>> biquadFilter;
//    std::mutex filterMutex;
//    
//} TestStruct2;
//
//typedef struct 
//{
//    int test;
//    std::array<TestStruct2, audioProcessor::beamProcessor::FILTER_COUNT> thirdOctaveFilters;
//} TestStruct;
//static std::array<TestStruct, audioProcessor::beamProcessor::BEAM_COUNT> _TestStructList;



bool audioProcessor::beamProcessor::init(ArrayConfiguration const &arrayConfig, IProcessedAudioResultListener *processResultListener)
{

    fmt::print("{}  c\n", kfr::library_version());

    _processResultListener = processResultListener;

    // create the filters
    for (auto &f : centerMicThirdOctaveFilters) { 
        kfr::zpk<float_t> filt = iir_bandpass(kfr::butterworth<float_t>(4), f.lowerFreqCutOff, f.upperFreqCutOff, SAMPLE_RATE_HZ); 
        f.secondOrderStagesCoefficients = to_sos(filt);
        f.biquadFilter.reset(new kfr::biquad_filter<float_t>(f.secondOrderStagesCoefficients));
    }

    // create a filter set for each beam
    //for (size_t i = 0; i < audioProcessor::beamProcessor::BEAM_COUNT; i++) 
    //{ 
    //    
    //}

    for (auto &beam : _beamFilterStates)
    { 
        for (size_t f = 0; f < audioProcessor::beamProcessor::FILTER_COUNT; f++) { 
            beam.thirdOctaveFilters[f].filterBandNumber = centerMicThirdOctaveFilters[f].filterBandNumber ;
            beam.thirdOctaveFilters[f].lowerFreqCutOff = centerMicThirdOctaveFilters[f].lowerFreqCutOff ;
            beam.thirdOctaveFilters[f].centerFreq = centerMicThirdOctaveFilters[f].centerFreq ;
            beam.thirdOctaveFilters[f].upperFreqCutOff = centerMicThirdOctaveFilters[f].upperFreqCutOff ;
            beam.thirdOctaveFilters[f].secondOrderStagesCoefficients = centerMicThirdOctaveFilters[f].secondOrderStagesCoefficients ;

            kfr::zpk<float_t> filt = iir_bandpass(kfr::butterworth<float_t>(4), beam.thirdOctaveFilters[f].lowerFreqCutOff, beam.thirdOctaveFilters[f].upperFreqCutOff, SAMPLE_RATE_HZ); 
            beam.thirdOctaveFilters[f].secondOrderStagesCoefficients = to_sos(filt);
            beam.thirdOctaveFilters[f].biquadFilter.reset(new kfr::biquad_filter<float_t>(beam.thirdOctaveFilters[f].secondOrderStagesCoefficients));

        }

    }

    _arrayConfig = arrayConfig;
    numberOfMics = _arrayConfig.numberOfMicsInnerRing + _arrayConfig.numberOfMicsOuterRing;
    calculateMicPositions();

    calculateSteeringDelays(0.0);
    return false;
}

void audioProcessor::beamProcessor::processAudioFrame(ProcessingParameters processingParameters, const AudioFrame &audioFrame)
{

    std::vector<ProcessResult> results;

    _processAudioFrame(processingParameters, audioFrame, results);

    if (_processResultListener != nullptr)
    {
        for (auto res: results) 
        {
            _processResultListener->OnProcessedAudioResult(res);   
        }
    }
}

// ===========================================================
// Private
// ===============================

void audioProcessor::beamProcessor::_processAudioFrame(ProcessingParameters processingParameters, const AudioFrame  &audioFrame, std::vector<ProcessResult> (&result))
{

    // const std::string options = "phaseresp=False, log_freq=True, freq_dB_lim=(-160, 10), padwidth=8192";
    //   kfr::plot_show("butterworth_bandpass4", output, options + ", title='4th-order Butterworth filter, bandpass'");
   
    static kfr::univector<float_t> summedResult    = kfr::univector<float_t>(audioProcessor::beamProcessor::BLOCK_SIZE);
    //static kfr::univector<float_t> centreMic       = kfr::univector<float_t>(audioProcessor::beamProcessor::BLOCK_SIZE);
    kfr::univector<float_t> centreMic    = kfr::make_univector(audioFrame.micSignals[0], audioProcessor::beamProcessor::BLOCK_SIZE);
    static kfr::univector<float_t> output          = kfr::univector<float_t>(audioProcessor::beamProcessor::BLOCK_SIZE);

    ProcessResult _result;

    // process centre mic
    for (size_t f = 0; f < FILTER_COUNT; f++) 
    {
        const std::lock_guard<std::mutex> lock(centerMicThirdOctaveFilters[f].filterMutex);
        centerMicThirdOctaveFilters[f].biquadFilter->apply(output, centreMic);
        _result.thirdOctaveBandsCentreMic[f] = kfr::rms(output);
    }

    for (int a  = 0; a < processingParameters.steeringAngles.size(); a++)
    {
        auto start = high_resolution_clock::now();
        _result.steeringAngle = processingParameters.steeringAngles[a];

        calculateSteeringDelays(processingParameters.steeringAngles[a]);
        sumMicSignals(audioFrame, summedResult);

        for (size_t f = 0; f < FILTER_COUNT; f++) 
        {
            const std::lock_guard<std::mutex> lock(_beamFilterStates[a].thirdOctaveFilters[f].filterMutex);
            //_beamFilterStates[a].thirdOctaveFilters[f].filterMutex.lock();
            _beamFilterStates[a].thirdOctaveFilters[f].biquadFilter->apply(output, summedResult);//summedBlockSize
            _result.thirdOctaveBandsSteeredBeam[f] = kfr::rms(output);
            //_beamFilterStates[a].thirdOctaveFilters[f].filterMutex.unlock();
        }
        
        auto stop = high_resolution_clock::now();
        _result.processingTime_us = duration_cast<microseconds>(stop - start).count();
        
        result.push_back(_result);
    }

}

void audioProcessor::beamProcessor::calculateMicPositions() {
    uint8_t micIndex = 0;
    _micPositions.clear();

    MicCoordinate_t _pos;
    for (uint8_t i = 0; i < _arrayConfig.numberOfMicsInnerRing; i++) {
        _pos.x = _arrayConfig.innerRingRadius * cosf(2 * audioProcessor::beamProcessor::PI * i / _arrayConfig.numberOfMicsInnerRing);
        _pos.y = _arrayConfig.innerRingRadius * sinf(2 * audioProcessor::beamProcessor::PI * i / _arrayConfig.numberOfMicsInnerRing);

        _micPositions.push_back(_pos);
        micIndex++;
    }
    for (uint8_t i = 0; i < _arrayConfig.numberOfMicsOuterRing; i++) {
        _pos.x = _arrayConfig.outerRingRadius * cosf(2 * audioProcessor::beamProcessor::PI * i / _arrayConfig.numberOfMicsOuterRing);
        _pos.y = _arrayConfig.outerRingRadius * sinf(2 * audioProcessor::beamProcessor::PI * i / _arrayConfig.numberOfMicsOuterRing);
        _micPositions.push_back(_pos);
        micIndex++;
    }
    
}
void audioProcessor::beamProcessor::calculateSteeringDelays(float_t steeringAngle_deg)
{
    float steeringAngle_radians = steeringAngle_deg / 180.0 * audioProcessor::beamProcessor::PI;

    _steeringDelays_s.assign(numberOfMics, 0.0);
    _steeringDelays_samples.assign(numberOfMics, 0);

    for (int i = 0; i < numberOfMics; i++) {

        _steeringDelays_s[i] = _micPositions[i].x * cosf(steeringAngle_radians) + _micPositions[i].y * sinf(steeringAngle_radians);
        _steeringDelays_s[i] /= audioProcessor::beamProcessor::SPEED_OF_SOUND;
        _steeringDelays_samples[i] = (int16_t)(round(_steeringDelays_s[i] * SAMPLE_RATE_HZ));
    }
}


/// <summary>
/// 
/// </summary>
/// <param name="audioFrame"></param>
/// <param name="summedResult"></param>
/// <param name="centreMic"></param>
/// <returns>the number of samples in the summed array. This is the number of samples that over lap all of the shifted signals.</returns>
uint16_t audioProcessor::beamProcessor::sumMicSignals(AudioFrame const &audioFrame, kfr::univector<float_t> (&summedResult))
{
    int16_t minDelay = INT16_MAX;
    int16_t maxDelay = INT16_MIN;

    if (_steeringDelays_samples.size() != numberOfMics)
    { 
        return 0;
    }


    for (int i = 0; i < numberOfMics; i++) {
        minDelay = std::fmin(_steeringDelays_samples[i], minDelay);
        maxDelay = std::fmax(_steeringDelays_samples[i], maxDelay);
    }

    uint16_t span = maxDelay - minDelay;
    uint16_t finalBlockSize = BLOCK_SIZE - span;

    for (uint16_t i = 0; i < finalBlockSize; ++i) {
        float_t sampleSum = 0;

        for (int m = 0; m < numberOfMics; m++) {
            //assert(SteeringDelays_samples[m] - minDelay >= 0);
            //assert(SteeringDelays_samples[m] - minDelay + i < BLOCK_SIZE);

            // mic 0 is the centre mic and it does not have a steering delay associated with it
            sampleSum += (float_t)audioFrame.micSignals[m + 1][_steeringDelays_samples[m] - minDelay + i]; 
        }

        summedResult[i] = sampleSum / (float_t)numberOfMics;
    }

    return finalBlockSize;
}