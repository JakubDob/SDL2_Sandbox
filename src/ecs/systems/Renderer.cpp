#include "Renderer.h"
#include "../components/Transform2D.h"
#include "../components/StaticSprite.h"
#include "../components/AnimatedSprite.h"

Renderer::Renderer(Texture const& whiteTexture) : whiteTexture(&whiteTexture) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ambientStrength = 0.01;
    lightColor = Vec3({1, 1, 1});
    // 9 = QuadVertex stride
    quadVbo.init<float>(9 * 4 * MAX_QUADS_PER_BATCH, GL_DYNAMIC_DRAW);
    std::vector<unsigned> indices;
    constexpr unsigned indexCount = MAX_QUADS_PER_BATCH * 6;
    indices.reserve(6 * MAX_QUADS_PER_BATCH);
    // create integer sequence 0,1,2, 2,3,4 | 4,5,6, 6,7,5 ...
    int trigCount = -1;
    for (unsigned i = 0; i < indexCount; ++i) {
        if (i % 3 == 0) {
            ++trigCount;
        }
        if ((i + 1) % 6 == 0) {
            indices.push_back(indices[i - 5]);
        } else {
            indices.push_back(i - trigCount);
        }
    }
    VertexBufferLayout layout;
    layout.add<float>(2, VertexAttributeInfo::POSITION);
    layout.add<float>(2, VertexAttributeInfo::TEX_COORD);
    layout.add<float>(2, VertexAttributeInfo::NORMAL_TEX_COORD);
    layout.add<float>(4, VertexAttributeInfo::COLOR);
    layout.add<float>(1, VertexAttributeInfo::DEPTH);
    quadEbo.init(indices);
    quadVao.init(quadVbo, quadEbo, layout);
}

size_t Renderer::addMesh(QuadMesh const& mesh) {
    meshes.push_back(mesh);
    return meshes.size() - 1;
}
QuadMesh& Renderer::getMesh(size_t index) { return meshes[index]; }

void Renderer::addRenderCommand(RenderCommand command) {
    if (!command.texture) {
        command.texture = whiteTexture;
    }
    if (command.hud) {
        hudObjects.push_back(command);
        return;
    }
    if (command.opaque) {
        opaqueObjects.emplace_back(command);
        return;
    } else {
        translucentObjects.emplace_back(command);
        return;
    }
}

void Renderer::renderOpaque() {
    int drawCalls = 0;
    std::sort(opaqueObjects.begin(), opaqueObjects.end(),
              [](RenderCommand const& a, RenderCommand const& b) {
                  return a.texture->getId() < b.texture->getId();
              });
    buffer.reserve(opaqueObjects.size() * 4);
    RenderCommand currentCommand = opaqueObjects[0];
    for (size_t i = 0; i < opaqueObjects.size(); ++i) {
        auto const& currentObj = opaqueObjects[i];
        auto nextI = i + 1;
        buffer.push_back(meshes[currentObj.meshId]);
        if (nextI == opaqueObjects.size() ||
            opaqueObjects[nextI] != currentCommand) {
            ++drawCalls;
            currentCommand.shader->use();
            currentCommand.shader->setVec3("lightColor", lightColor);
            currentCommand.shader->setVec2("lightSourcePos", lightSourcePos);
            currentCommand.shader->setFloat("ambientStrength", ambientStrength);
            currentCommand.texture->setActiveAndBind(0);
            quadVbo.update(0, buffer);
            glDrawElements(GL_TRIANGLES, buffer.size() * 6, GL_UNSIGNED_INT,
                           nullptr);
            buffer.clear();
            if (nextI < opaqueObjects.size()) {
                currentCommand = opaqueObjects[nextI];
            }
        }
    }
    glUseProgram(0);
    opaqueObjects.clear();
}

void Renderer::renderTranslucent() {
    int drawCalls = 0;
    std::sort(translucentObjects.begin(), translucentObjects.end(),
              [](RenderCommand const& a, RenderCommand const& b) {
                  return a.state.depthState.depth > b.state.depthState.depth;
              });
    RenderCommand currentCommand = translucentObjects[0];
    for (size_t i = 0; i < translucentObjects.size(); ++i) {
        auto const& currentObj = translucentObjects[i];
        buffer.push_back(meshes[currentObj.meshId]);
        auto nextI = i + 1;
        if (nextI == translucentObjects.size() ||
            translucentObjects[nextI] != currentCommand) {
            ++drawCalls;
            currentCommand.shader->use();
            currentCommand.shader->setVec3("lightColor", lightColor);
            currentCommand.shader->setVec2("lightSourcePos", lightSourcePos);
            currentCommand.shader->setFloat("ambientStrength", ambientStrength);
            currentCommand.shader->setBool("enableLight", m_enableLight);
            currentCommand.texture->setActiveAndBind(0);
            quadVbo.update(0, buffer);
            glDrawElements(GL_TRIANGLES, buffer.size() * 6, GL_UNSIGNED_INT,
                           nullptr);
            buffer.clear();
            if (nextI < translucentObjects.size()) {
                currentCommand = translucentObjects[nextI];
            }
        }
    }
    glUseProgram(0);
    translucentObjects.clear();
}

void Renderer::tryEnableScrissor(ScissorState const& state) {
    if (state.enabled) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(state.rect[0], state.rect[1], state.rect[2], state.rect[3]);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void Renderer::renderHudObjects() {
    // back to front
    std::sort(hudObjects.begin(), hudObjects.end(),
              [](RenderCommand const& a, RenderCommand const& b) {
                  return (a.windowElement < b.windowElement) ||
                         ((a.windowElement == b.windowElement) &&
                          (a.focusLayer < b.focusLayer)) ||
                         ((a.windowElement == b.windowElement) &&
                          (a.focusLayer == b.focusLayer) &&
                          (a.layer < b.layer)) ||
                         ((a.windowElement == b.windowElement) &&
                          (a.focusLayer == b.focusLayer) &&
                          (a.layer == b.layer) && (a.id < b.id));
              });
    RenderCommand currentCommand = hudObjects[0];
    for (size_t i = 0; i < hudObjects.size(); ++i) {
        auto const& currentObj = hudObjects[i];
        buffer.push_back(meshes[currentObj.meshId]);
        auto nextI = i + 1;
        if (nextI == hudObjects.size() || hudObjects[nextI] != currentCommand) {
            currentCommand.shader->use();
            currentCommand.texture->setActiveAndBind(0);
            tryEnableScrissor(currentCommand.state.scissorState);
            quadVbo.update(0, buffer);
            glDrawElements(GL_TRIANGLES, buffer.size() * 6, GL_UNSIGNED_INT,
                           nullptr);
            buffer.clear();
            if (nextI < hudObjects.size()) {
                currentCommand = hudObjects[nextI];
            }
        }
    }
    glUseProgram(0);
    hudObjects.clear();
}

void Renderer::setLightColor(Vec3 const& color) { this->lightColor = color; }

void Renderer::setLightPosition(Vec2 const& pos) { this->lightSourcePos = pos; }

void Renderer::enableLight(bool value) { this->m_enableLight = value; }

void Renderer::toggleLight() { this->m_enableLight = !m_enableLight; }

void Renderer::batchRender() {
    quadVao.bind();
    glEnable(GL_DEPTH_TEST);

    if (!opaqueObjects.empty()) {
        renderOpaque();
    }

    if (!translucentObjects.empty()) {
        renderTranslucent();
    }

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_SCISSOR_TEST);
    if (!hudObjects.empty()) {
        renderHudObjects();
    }
    glDisable(GL_SCISSOR_TEST);

    meshes.clear();
    quadVao.unbind();
}