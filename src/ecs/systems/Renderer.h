#pragma once
#include "../EcsContainer.h"
#include "../../opengl/Shader.h"
#include "../../opengl/Texture.h"
#include "../../opengl/VertexArray.h"
#include "../../opengl/VertexBuffer.h"
#include "../../opengl/ElementBuffer.h"
#include "../../Types.h"
#include "../../QuadMesh.h"
#include <vector>

class AnimatedSprite;
inline constexpr int MAX_QUADS_PER_BATCH = 256;

struct ScissorState {
    void set(GeometryRect const& r) {
        enabled = true;
        rect = r;
    }
    bool enabled = false;
    GeometryRect rect;
};
struct DepthState {
    void set(float value) {
        enabled = true;
        depth = value;
    }
    bool enabled = false;
    float depth = 0;
};
struct RendererState {
    ScissorState scissorState;
    DepthState depthState;
};

class RenderCommand {
   public:
    RenderCommand() = default;
    RenderCommand(int meshId, Shader const& shader)
        : meshId(meshId), shader(&shader) {}
    Shader const* shader = nullptr;
    Texture const* texture =
        nullptr;  // Renderer passes white texture here if not provided
    RendererState state{};
    int meshId = 0;
    int layer = 0;
    // distinguish groups (all elements belonging to the same window have the
    // same id) of hud objects when batching with scissors (cannot batch two
    //windows with two different scissors) comparing scissors is not enough if
    // two windows lay on top of each other and have the same scrissor
    // rectangles
    int id = 0;
    bool opaque = false;
    bool hud = false;         // separate hud and non hud elements
    bool focusLayer = false;  // focused window always on top of other windows
    bool windowElement = false;  // render all windows before free elements (eg.
                                 // non windowed buttons)
};

inline bool operator!=(ScissorState const& a, ScissorState const& b) {
    return (a.enabled != b.enabled) || (a.rect[0] != b.rect[0]) ||
           (a.rect[1] != b.rect[1]) || (a.rect[2] != b.rect[2]) ||
           (a.rect[3] != b.rect[3]);
}

inline bool operator!=(DepthState const& a, DepthState const& b) {
    return (a.enabled != b.enabled);  // values don't matter when comparing,
                                      // only if it is enabled or not
}
inline bool operator!=(RendererState const& a, RendererState const& b) {
    return (a.depthState != b.depthState) || (a.scissorState != b.scissorState);
}

inline bool operator!=(RenderCommand const& a, RenderCommand const& b) {
    return (a.shader != b.shader) || (a.texture != b.texture) ||
           (a.layer != b.layer) || (a.id != b.id) || (a.state != b.state);
}

class Renderer {
   public:
    Renderer(Texture const& whiteTexture);
    void batchRender();
    void addRenderCommand(RenderCommand command);
    size_t addMesh(QuadMesh const& mesh);
    QuadMesh& getMesh(size_t index);
    void setLightPosition(Vec2 const& pos);
    void setLightColor(Vec3 const& color);
    void enableLight(bool value);
    void toggleLight();

   private:
    void renderOpaque();
    void renderTranslucent();
    void renderHudObjects();
    void tryEnableScrissor(ScissorState const& state);

    std::vector<QuadMesh> meshes;
    std::vector<RenderCommand> opaqueObjects;       // sort by texture id
    std::vector<RenderCommand> translucentObjects;  // sort by texture id
    std::vector<RenderCommand> hudObjects;
    std::vector<QuadMesh> buffer;
    Texture const* whiteTexture;

    Vec3 lightColor;
    Vec2 lightSourcePos;
    float ambientStrength;
    bool m_enableLight = true;

    VertexArray quadVao;
    VertexBuffer quadVbo;
    ElementBuffer quadEbo;
};
