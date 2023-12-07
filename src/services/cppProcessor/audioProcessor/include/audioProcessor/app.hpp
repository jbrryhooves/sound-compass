#pragma once

#include <audioProcessor/audioProcessor.hpp>


namespace audioProcessor {
class App
{
  public:
    App(){}
    ~App() {}
    bool init();
    
    /// <summary>
    /// Never returns
    /// </summary>
    void run();

  private:
    audioProcessor _processor;
};

}