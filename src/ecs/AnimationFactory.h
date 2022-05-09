#pragma once
#include "components/AnimatedSprite.h"

namespace ecs {
class AnimationFactory {
   public:
    static AnimatedSprite::FsmType createBasicAnimation();
};
}  // namespace ecs