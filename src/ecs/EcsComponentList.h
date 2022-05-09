#pragma once
#include "../Types.h"

namespace ecs {
struct Physics2DTag {};
struct Transform2DTag {};
struct Graphics2DTag {};
struct Collider2DTag {};
struct Controller2DTag {};
struct HealthBarTag {};
struct StaticSpriteTag {};
struct AnimatedSpriteTag {};
struct AnimationTag {};

// order in the list defines layout in memory
using ComponentTags =
    TypeSequence<Transform2DTag, Physics2DTag, StaticSpriteTag,
                 AnimatedSpriteTag, Controller2DTag, Graphics2DTag,
                 Collider2DTag, AnimationTag, HealthBarTag>;
}  // namespace ecs
