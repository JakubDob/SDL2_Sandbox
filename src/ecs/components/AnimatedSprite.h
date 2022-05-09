#pragma once
#include "../EcsContainer.h"
#include "../EcsComponentList.h"
#include "../../FiniteStateMachine.h"
#include "../../QuadMesh.h"
#include "../../TextureAtlasFrame.h"

class Texture;
namespace animation {
// useful constants to use with state machine
inline constexpr uint_fast8_t IDLE = 0;
inline constexpr uint_fast8_t WALK = 1;
inline constexpr uint_fast8_t JUMP = 2;
inline constexpr uint_fast8_t RUN = 3;
inline constexpr uint_fast8_t ATTACK_0 = 4;
inline constexpr uint_fast8_t ATTACK_1 = 5;

inline constexpr uint_fast8_t MAX_FRAMES = 32;
inline constexpr uint_fast8_t TOTAL_STATES = 8;
}  // namespace animation

namespace ecs {
class AnimatedSprite
    : public ComponentBase<AnimatedSprite, AnimatedSpriteTag, ComponentTags> {
   public:
    using TransitionFunction = bool (*)(ecs::EcsContainer&, ecs::Entity);
    using ActionFunction = void (*)(ecs::EcsContainer&, ecs::Entity,
                                    int currentFrameOfState);
    using FsmType = FiniteStateMachine<animation::TOTAL_STATES,
                                       TransitionFunction, ActionFunction>;
    using AnimationEndFunction = void (*)(ecs::EcsContainer&, ecs::Entity,
                                          int playCount);

    AnimatedSprite(FsmType fsm, Texture const& atlasTexture, scalar_t width,
                   scalar_t height);
    QuadMesh const& getMesh() const;
    Texture const* getTexture() const;
    inline void setFps(float fps) { this->fps = fps; }
    inline auto& getFiniteStateMachine() { return fsm; }
    template <uint_fast8_t State, typename... FrameDataType,
              typename = std::enable_if_t<std::conjunction_v<
                  std::is_same<FrameDataType, TextureAtlasFrame>...>>>
    inline void setFramesFor(FrameDataType const&... args) {
        static_assert(State < animation::TOTAL_STATES, "State id out of range");
        stateMap[State].begin = totalFrameCount;
        stateMap[State].end = totalFrameCount + sizeof...(FrameDataType);
        uint_fast8_t i = 0;
        ((meshes[stateMap[State].begin + i++] = parseAtlasFrame(args)), ...);
        totalFrameCount += sizeof...(FrameDataType);
    }

    QuadMesh parseAtlasFrame(TextureAtlasFrame const& frame);

    template <uint_fast8_t State>
    inline void onEnd(AnimationEndFunction fn) {
        static_assert(State < animation::TOTAL_STATES, "State id out of range");
        onEndFuncs[State] = fn;
    }

    inline int getFullPlayCount() const { return fullPlayCount; }
    inline int getCurrentFrame() const { return currentFrameOfState; }
    inline bool isOpaque() const { return opaque; }
    void rotate(scalar_t degreeAngle);
    void removeUnusedStates();
    void play(ecs::EcsContainer& ecsContainer, ecs::Entity const& entity);
    void update(ecs::EcsContainer& ecsContainer, ecs::Entity const& entity,
                scalar_t dt);
    void reset();

   private:
    struct AnimationState {
        uint_fast8_t begin = 0;
        uint_fast8_t end = 0;
    };

    inline int getFrameCount(uint_fast8_t state) {
        return stateMap[state].end - stateMap[state].begin;
    }
    FsmType fsm;
    std::array<AnimationState, animation::TOTAL_STATES> stateMap{};
    std::array<QuadMesh, animation::MAX_FRAMES>
        meshes{};  // frames from an atlas
    std::array<AnimationEndFunction, animation::TOTAL_STATES> onEndFuncs;
    Texture const* texture = nullptr;
    scalar_t width;
    scalar_t height;
    scalar_t totalTimeForCurrentState = 0;
    scalar_t fps = 1;
    int fullPlayCount = 0;
    int currentFrameI = 0;
    int currentFrameOfState = 0;
    int previousFrameOfState = 0;
    uint_fast8_t totalFrameCount = 0;
    bool opaque;
};
}  // namespace ecs
