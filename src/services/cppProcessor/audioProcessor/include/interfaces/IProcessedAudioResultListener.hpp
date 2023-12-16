#pragma once

#include <audioProcessor/beamProcessor.hpp>


namespace audioProcessor {
class IProcessedAudioResultListener
{
  public:
    
    virtual void OnProcessedAudioResult(audioProcessor::beamProcessor::ProcessResult result) = 0;
};
}// namespace audioProcessor