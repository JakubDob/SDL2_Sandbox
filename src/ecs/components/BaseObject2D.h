#pragma once
#include "../../Types.h"

class BaseObject2D {
   public:
    virtual void update(scalar_t dt) = 0;
    virtual ~BaseObject2D() {}

   protected:
    BaseObject2D(){};

   private:
};