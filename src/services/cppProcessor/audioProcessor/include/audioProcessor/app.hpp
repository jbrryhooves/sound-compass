#pragma once

#include <interfaces/IProcessedAudioResultListener.hpp>
#include <audioProcessor/beamProcessor.hpp>


namespace audioProcessor {
class App : public IProcessedAudioResultListener
{
  public:
    App(){}
    ~App() {}
    bool init();
    
    /// <summary>
    /// Never returns
    /// </summary>
    void run();

    // IProcessedAudioResultListener
    void OnProcessedAudioResult(beamProcessor::ProcessResult result);

  private:
    beamProcessor _processor;
};

}