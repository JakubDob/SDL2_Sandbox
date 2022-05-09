#include "SandboxOptionsWindow.h"
#include "GuiButton.h"
#include "../EngineConstants.h"

namespace Gui {
SandboxOptionsWindow::SandboxOptionsWindow(int x, int y, int w, int h,
                                           DrawableText const& drawableText)
    : GuiWindow(x, y, w, h, "Sandbox Options", drawableText) {}
}  // namespace Gui