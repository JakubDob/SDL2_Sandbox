#include "TimeUtils.h"
#include <SDL2/SDL.h>

TimeUtils::TimeUtils() : past(SDL_GetTicks()) {}

void TimeUtils::setMaxFPS(int maxFPS) {
    if (maxFPS < 0) maxFPS = -maxFPS;
    maxFrameTime = 1000.f / maxFPS;
}

void TimeUtils::calcFPS() {
    curr = SDL_GetTicks();
    deltaTime = (Time)(curr - past) / (Time)1000;
    past = curr;
    deltaTimeSum += deltaTime;
    totalTime += deltaTime;
    frameCount++;
    if (deltaTimeSum >= 1) {
        currentFps = frameCount;
        frameCount = 0;
        deltaTimeSum = 0;
    }
    if (currentFps != pastFps) {
        pastFps = currentFps;
        fpsText = "fps: " + std::to_string(currentFps);
    }
}

void TimeUtils::throttleFPS() {
    float time = ceilf(maxFrameTime - deltaTime);
    if (time > 0) {
        SDL_Delay(static_cast<int>(time));
    }
}