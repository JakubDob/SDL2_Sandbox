#pragma once
#include <cstdio>
#include <string>
#include "Types.h"
class TimeUtils {
   public:
    TimeUtils();
    void setMaxFPS(int maxFPS);
    // calculates current frames per second, dt = delta time
    void calcFPS();
    void throttleFPS();
    inline std::string const& getFpsText() { return fpsText; }
    inline Time const& getDt() const { return deltaTime; }
    inline Time const& getTotal() const { return totalTime; }

   private:
    Time deltaTime = 0;
    Time deltaTimeSum = 0;
    Time totalTime = 0;
    int frameCount = 0;
    float maxFrameTime = 0;
    int currentFps = 0;
    int pastFps = 0;
    std::string fpsText;
    unsigned curr = 0;
    unsigned past;
};