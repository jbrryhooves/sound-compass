#pragma once

namespace audioProcessor {
class audioProcessor
{
  public:
    struct AudioFrame
    {

    };
    struct ProcessResult
    {

    };

    //audioProcessor();
    //~audioProcessor();

    bool init();

    void processAudioFrame(AudioFrame& audioFrame, ProcessResult& result);

  private:
};
//
//audioProcessor::audioProcessor() {}
//
//audioProcessor::~audioProcessor() {}
}