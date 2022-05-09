#include "AnimatedSprite.h"

namespace ecs {
AnimatedSprite::AnimatedSprite(FsmType fsm, Texture const& atlasTexture,
                               scalar_t width, scalar_t height)
    : fsm(fsm), texture(&atlasTexture), width(width), height(height) {
    onEndFuncs.fill(nullptr);
    opaque = false;
}

QuadMesh const& AnimatedSprite::getMesh() const {
    return meshes[currentFrameI];
}

Texture const* AnimatedSprite::getTexture() const { return texture; }

void AnimatedSprite::update(ecs::EcsContainer& ecsContainer,
                            ecs::Entity const& entity, scalar_t dt) {
    totalTimeForCurrentState += dt;
    if (fsm.update(ecsContainer, entity)) {
        reset();
    }
}

void AnimatedSprite::play(ecs::EcsContainer& ecsContainer,
                          ecs::Entity const& entity) {
    auto currState = fsm.getCurrentState();
    auto frameCount = getFrameCount(currState);
    currentFrameOfState =
        static_cast<int>(totalTimeForCurrentState * fps) % frameCount;
    currentFrameI = stateMap[currState].begin + currentFrameOfState;
    if (currentFrameOfState != previousFrameOfState) {
        previousFrameOfState = currentFrameOfState;
        if (currentFrameOfState == 0) {
            fullPlayCount++;
            if (onEndFuncs[currState]) {
                onEndFuncs[currState](ecsContainer, entity, fullPlayCount);
            }
        }
    }
    fsm.performAction(ecsContainer, entity, currentFrameOfState);
}

void AnimatedSprite::removeUnusedStates() {
    for (int i = 0; i < animation::TOTAL_STATES; ++i) {
        if (getFrameCount(i) == 0) {
            fsm.makeStateInaccessible(i);
        }
    }
}

void AnimatedSprite::reset() {
    totalTimeForCurrentState = 0;
    fullPlayCount = 0;
    currentFrameOfState = 0;
    previousFrameOfState = 0;
}

QuadMesh AnimatedSprite::parseAtlasFrame(TextureAtlasFrame const& frame) {
    return frame.createMesh(TextureAtlasFrame::center, width, height, true);
}

void AnimatedSprite::rotate(scalar_t degreeAngle) {
    for (auto& mesh : meshes) {
        mesh.transformPosition(math::getRotationMatrix(degreeAngle));
    }
}
}  // namespace ecs
