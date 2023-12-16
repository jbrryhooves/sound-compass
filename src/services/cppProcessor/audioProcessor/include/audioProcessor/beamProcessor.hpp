#pragma once
#include <kfr/base.hpp>
//#include <interfaces/IProcessedAudioResultListener.hpp>


namespace audioProcessor {

// forward decl
class IProcessedAudioResultListener;

class beamProcessor
{

  public:
    static const uint16_t BLOCK_SIZE = 960;// 30 1/3rd octave bands with 480 samples.
    static const int FILTER_COUNT = 30;
    static const int BEAM_COUNT = 50;
    static constexpr float SPEED_OF_SOUND = 343.0;
    static constexpr double PI = 3.14159265358979323846;

    typedef struct
    {
        std::time_t timestamp;

        float_t micSignals[16][BLOCK_SIZE];

    } AudioFrame;

    typedef struct
    {

        std::time_t timestamp;
        float thirdOctaveBandsSteeredBeam[FILTER_COUNT];
        float thirdOctaveBandsCentreMic[FILTER_COUNT];
        float steeringAngle;
        uint16_t processingTime_us;

    } ProcessResult;

    typedef struct
    {
        // relative to the centre of the circle.
        float x;
        float y;
    } MicCoordinate_t;

    typedef struct
    {
        // relative to the centre of the circle.
        uint16_t numberOfMicsInnerRing;
        uint16_t numberOfMicsOuterRing;
        float innerRingRadius;
        float outerRingRadius;

    } ArrayConfiguration;

    typedef struct
    {
        std::vector<float> steeringAngles;


    } ProcessingParameters;


    bool init(ArrayConfiguration const &arrayConfig, IProcessedAudioResultListener *processResultListener);


    void processAudioFrame(ProcessingParameters processingParameters, const AudioFrame &audioFrame);

  private:
    IProcessedAudioResultListener *_processResultListener;
    ArrayConfiguration _arrayConfig;
    std::vector<MicCoordinate_t> _micPositions;
    uint8_t numberOfMics;

    std::vector<float> _steeringDelays_s;
    std::vector<int16_t> _steeringDelays_samples;

    void calculateMicPositions();
    void calculateSteeringDelays(float_t steeringAngle_deg);
    uint16_t sumMicSignals(AudioFrame const &audioFrame, kfr::univector<float_t>(&summedResult));
    void _processAudioFrame(ProcessingParameters processingParameters, const AudioFrame &audioFrame, std::vector<ProcessResult>(&result));
};
//
// audioProcessor::audioProcessor() {}
//
// audioProcessor::~audioProcessor() {}
}// namespace audioProcessor