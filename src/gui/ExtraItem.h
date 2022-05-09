#pragma once
#include "../opengl/Texture.h"
#include "../TextureAtlasFrame.h"
#include "../Types.h"
// class for elements that are part of a widget but require no additional
// customization (no alignment/padding/text) like scrollbar thumbs
// TODO: implement
namespace Gui {
class ExtraItem {
   public:
    void setPosition(scalar_t x, scalar_t y);
    void setWidth(scalar_t w);
    void setHeight(scalar_t h);
    void setTexture(Texture const& texture);
    void setTexture(Texture const& texture, TextureAtlasFrame const& frame);

   private:
    GeometryRect renderQuad;
    Vec4 color;
    Texture* texture;
    TextureAtlasFrame* frame;
    bool usesFrame = false;
};

}  // namespace Gui