#pragma once
#include "GuiWindow.h"

namespace Gui {
class SandboxOptionsWindow final : public GuiWindow {
   public:
    SandboxOptionsWindow(int x, int y, int w, int h,
                         DrawableText const& drawableText);
};
}  // namespace Gui