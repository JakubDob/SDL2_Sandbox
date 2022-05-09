#include "Engine2D.h"
#include "Utils.h"
#include "EngineConstants.h"
#include "gui/GuiButton.h"
#include "gui/SandboxOptionsWindow.h"
#include "opengl/VertexBufferLayout.h"
#include "opengl/VertexArray.h"
#include "opengl/UniformBuffer.h"
#include "ecs/PrefabFactory.h"
#include "ecs/components/Controller2D.h"
#include "ecs/components/Physics2D.h"
#include "ecs/components/Collider2D.h"
#include "ecs/components/Transform2D.h"
#include "ecs/components/HealthBar.h"
#include "ecs/components/BoxCollider.h"
#include "ecs/components/StaticSprite.h"
#include "ecs/EcsComponentList.h"
#include "ecs/AnimationFactory.h"
#include "DrawableText.h"
#include <glad/glad.h>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <SDL2/SDL.h>
#include <string>
#include <filesystem>
#include "Debug.h"

Engine2D& Engine2D::getInstance() {
    static Engine2D engine;
    return engine;
}
AssetsManager& Engine2D::getAssetsManager() { return assetsManager; }
Engine2D::Engine2D() : ecsContainer(ecs::ComponentTags{}) {
    std::string err;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        err = SDL_GetError();
        throw std::runtime_error("SDL_Init error: " + err);
    }
    if (TTF_Init() < 0) {
        err = SDL_GetError();
        throw std::runtime_error("TTF_Init error: " + err);
    }
    try {
        this->window = std::make_unique<Window>(
            "Sandbox", EngineConstants::Window::width,
            EngineConstants::Window::height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    } catch (std::exception& e) {
        err = e.what();
        throw std::runtime_error("window error: " + err);
    }
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        err = IMG_GetError();
        throw std::runtime_error("IMG_Init error: " + err);
    }

    renderer = std::make_unique<Renderer>(
        assetsManager.create1x1RGBATexture(0xffffffff, "white"));
    SDL_GL_SetSwapInterval(1);
}

ecs::EcsContainer& Engine2D::getEcs() { return ecsContainer; }

ecs::PhysicsSystem& Engine2D::getPhysicsSystem() { return physicsSystem; }

Camera2D& Engine2D::getCamera() { return camera; }
Renderer& Engine2D::getRenderer() { return *renderer; }

Gui::GuiSystem& Engine2D::getGuiSystem() { return guiSystem; }
void Engine2D::setPause(bool value) { this->paused = value; }

void Engine2D::setControlledEntity(ecs::Entity const& entity) {
    this->controlledObj = entity;
}

Shader& Engine2D::getShader(ShaderType type) {
    switch (type) {
        case ShaderType::DEFAULT:
            return assetsManager.getShaderProgram("batchShader");
            break;
        case ShaderType::HP:
            return assetsManager.getShaderProgram("hpShader");
            break;
        case ShaderType::HUD:
            return assetsManager.getShaderProgram("hudShader");
            break;
        default:
            throw std::runtime_error("Shader type not implemented");
    }
}
Window& Engine2D::getWindow() { return *window; }

InputHandler& Engine2D::getInputHandler() { return inputHandler; }
ecs::AiSystem& Engine2D::getAiSystem() { return aiSystem; }
void Engine2D::onInit(init_fn fn) { onInitCallback = fn; }

void Engine2D::onUpdate(update_fn fn) { onUpdateCallback = fn; }

void Engine2D::onWindowSizeChange(window_event_fn fn) {
    onWindowSizeChangeCallback = fn;
}
void Engine2D::setOrto(scalar_t left, scalar_t right, scalar_t bottom,
                       scalar_t top) {
    ortoCenter = utils::ortographicProjection(left, right, bottom, top);
}

void Engine2D::quit() { m_quit = true; }

void Engine2D::pollEvents() {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            m_quit = true;
        } else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                window->setWidth(e.window.data1);
                window->setHeight(e.window.data2);
                glViewport(0, 0, e.window.data1, e.window.data2);
                // SDL style coord system with (0,0) being top left
                ortoGui = utils::ortographicProjection(0, window->getWidthF(),
                                                       window->getHeightF(), 0);

                if (onWindowSizeChangeCallback) {
                    onWindowSizeChangeCallback(e.window.data1, e.window.data2);
                }
            }
        } else {
            inputHandler.processEvent(e);
        }
    }
    inputHandler.checkKeys();
}
void Engine2D::initializeAssets(std::vector<std::string> const& assetsPaths) {
    std::string folder, asset, token, folderPath;
    char delimiter = EngineConstants::Path::delimiter;
    std::vector<std::string> tokens;
    for (auto const& p : assetsPaths) {
        std::istringstream ss(p);
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 2) {
            asset = tokens.back();
            tokens.pop_back();
            folder = tokens.back();
            for (auto const& token : tokens) {
                folderPath += token + delimiter;
            }

            if (folder == EngineConstants::Path::texturesFolder) {
                assetsManager.loadTexture(folderPath, asset);
            } else if (folder == EngineConstants::Path::fontsFolder) {
                assetsManager.loadFont(folderPath, asset);
            } else if (folder == EngineConstants::Path::shadersFolder) {
                assetsManager.loadShader(folderPath, asset);
            } else if (folder == EngineConstants::Path::atlasFolder) {
                assetsManager.loadAtlas(folderPath, asset);
            } else {
                throw std::runtime_error("Cannot load asset inside folder " +
                                         folder +
                                         ". The location was not specified.");
            }
            tokens.clear();
            folderPath.clear();
        } else {
            throw std::runtime_error(
                "Incorrect asset path: An asset should be placed in a folder "
                "with a predefined name.");
        }
    }

    assetsManager.saveShaderProgram("hudShader", "hudVertexShader",
                                    "hudFragmentShader");
    assetsManager.saveShaderProgram("batchShader", "batchVertexShader",
                                    "batchFragmentShader");
}

void Engine2D::initializeAssetsFromFileSystem(std::string path) {
    std::vector<std::string> paths;
    for (auto const& entry : std::filesystem::directory_iterator(path)) {
        for (auto const& assetPath :
             std::filesystem::directory_iterator(entry)) {
            std::string strPath = assetPath.path().string();
            std::replace(strPath.begin(), strPath.end(), '\\',
                         EngineConstants::Path::delimiter);
            paths.emplace_back(strPath);
        }
    }
    initializeAssets(paths);
}
void Engine2D::initializeScene() {
    Vec2 worldSize{100, 100};
    this->collisionSystem = std::make_unique<ecs::CollisionSystem2D>(worldSize);

#ifdef __ANDROID__
    inputHandler.addPointerMoveCallback([&](SDL_MouseMotionEvent const& e) {
        if (!ecsContainer.exists(controlledObj)) {
            return;
        }

        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (!c) {
            return;
        }
        if (e.xrel < 0)
            c->moveLeft(ecsContainer, timeUtils.getDt());
        else if (e.xrel > 0)
            c->moveRight(ecsContainer, timeUtils.getDt());
        if (e.yrel > 0)
            c->moveDown(ecsContainer, timeUtils.getDt());
        else if (e.yrel < 0)
            c->moveUp(ecsContainer, timeUtils.getDt());
    });
#endif
    auto scaleUp = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->scaleUp(ecsContainer, timeUtils.getDt());
    };
    auto scaleDown = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->scaleDown(ecsContainer, timeUtils.getDt());
    };
    auto rotateCCw = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused)
            c->rotateCounterClockwise(ecsContainer, timeUtils.getDt());
    };
    auto rotateCw = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->rotateClockwise(ecsContainer, timeUtils.getDt());
    };
    auto moveLeft = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->moveLeft(ecsContainer, timeUtils.getDt());
    };
    auto moveRight = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->moveRight(ecsContainer, timeUtils.getDt());
    };
    auto moveUp = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->moveUp(ecsContainer, timeUtils.getDt());
    };
    auto moveDown = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->moveDown(ecsContainer, timeUtils.getDt());
    };
    auto camZoomOut = [&] { camera.scaleZoom(0.99); };
    auto camZoomIn = [&] { camera.scaleZoom(1.01); };
    auto attack1 = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->attackType = 1;
    };
    auto attack2 = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) c->attackType = 2;
    };
    auto stop = [&] {
        auto* p = ecsContainer.getComponent<Physics2D>(controlledObj);
        if (p && !paused) p->setLinearVelocity({0, 0});
    };
    auto hurt = [&] {
        auto* hp = ecsContainer.getComponent<HealthBar>(controlledObj);
        if (hp && !paused) {
            hp->changeHp(-100);
        }
    };
    auto heal = [&] {
        auto* hp = ecsContainer.getComponent<HealthBar>(controlledObj);
        if (hp && !paused) {
            hp->changeHp(10);
        }
    };
    auto destroy = [&] {
        auto* c = ecsContainer.getComponent<Controller2D>(controlledObj);
        if (c && !paused) {
            auto entity = c->getEntity();
            ecsContainer.removeEntity(entity);
            std::cout << "Current entity count: "
                      << ecsContainer.getCurrentEntityCount() << "\n";
        }
    };
    auto toggleWindowVisibility = [&]() {
        bool value = this->sandboxOptionsWindow->isVisible();
        this->sandboxOptionsWindow->setVisible(!value);
    };
    auto togglePause = [&]() { this->paused = !this->paused; };
    auto quitApp = [&]() { m_quit = true; };
    auto toggleMenuVisible = [&] {
        bool visible = sandboxOptionsWindow->isVisible();
        sandboxOptionsWindow->setVisible(!visible);
    };
    auto someDebugInfo = [&]() {};
    inputHandler.addMultiGestureCallback(
        MultiGestureEvent::PINCH_OPEN,
        [&](SDL_MultiGestureEvent const& e) { camera.scaleZoom(1.01); });
    inputHandler.addMultiGestureCallback(
        MultiGestureEvent::PINCH_CLOSE,
        [&](SDL_MultiGestureEvent const& e) { camera.scaleZoom(0.99); });
    inputHandler.addKey(SDL_SCANCODE_W, scaleUp);
    inputHandler.addKey(SDL_SCANCODE_S, scaleDown);
    inputHandler.addKey(SDL_SCANCODE_A, rotateCCw);
    inputHandler.addKey(SDL_SCANCODE_D, rotateCw);
    inputHandler.addKey(SDL_SCANCODE_LEFT, moveLeft);
    inputHandler.addKey(SDL_SCANCODE_RIGHT, moveRight);
    inputHandler.addKey(SDL_SCANCODE_UP, moveUp);
    inputHandler.addKey(SDL_SCANCODE_DOWN, moveDown);
    inputHandler.addKey(SDL_SCANCODE_MINUS, camZoomOut);
    inputHandler.addKey(SDL_SCANCODE_EQUALS, camZoomIn);
    inputHandler.addKey(SDL_SCANCODE_Z, attack1);
    inputHandler.addKey(SDL_SCANCODE_X, attack2);
    inputHandler.addKey(SDL_SCANCODE_C, stop);
    inputHandler.addKey(SDL_SCANCODE_G, hurt);
    inputHandler.addKey(SDL_SCANCODE_H, heal);
    inputHandler.addKey(SDL_SCANCODE_J, destroy);
    inputHandler.addKey(SDL_SCANCODE_I, someDebugInfo);
    inputHandler.addKey(SDL_SCANCODE_P, togglePause, true);

    auto changeActiveObj = [&]() {
        auto& components =
            ecsContainer.getEntitiesWithComponents<Controller2D>();
        if (components.size() == 0) return;
        std::vector<ecs::Entity> ents;
        bool found = false;
        for (auto& [controller] : components) {
            auto* physics =
                ecsContainer.getComponent<Physics2D>(controller->getEntity());
            if (!physics || (physics && !physics->isStatic())) {
                ents.emplace_back(controller->getEntity());
            }
        }
        std::sort(ents.begin(), ents.end());
        for (auto& e : ents) {
            if (e.getId() > cameraAttachedObj.getId()) {
                cameraAttachedObj = e;
                found = true;
                break;
            }
        }
        if (!found && !ents.empty()) {
            cameraAttachedObj = ents[0];
        }
        camera.attach(ecsContainer, cameraAttachedObj);
        controlledObj = cameraAttachedObj;
        std::cout << "Found " << components.size()
                  << " controllable objects, current entity id: "
                  << controlledObj.getId() << "\n";
    };
    auto resetAllObj = [&]() {
        float xOffset = 1;
        //-platformW/2
        float xBegin = -50 / 2 + xOffset;
        auto& components =
            ecsContainer.getEntitiesWithComponents<Physics2D, Transform2D>();
        std::cout << "Reset all, got components: " << components.size() << "\n";
        for (auto& [physics, transform] : components) {
            if (!physics->isStatic()) {
                physics->setLinearVelocity({0, 0});
                transform->setPosition({xBegin, 5});
                xBegin += xOffset;
            }
        }
        for (auto& hp : ecs::ForEachComponent<HealthBar>(ecsContainer)) {
            hp.reset();
        }
    };

    auto toggleVSync = [&]() {
        vsyncEnabled = !vsyncEnabled;
        vsyncEnabled ? SDL_GL_SetSwapInterval(1) : SDL_GL_SetSwapInterval(0);
    };
    auto flipY = [&]() {
        ecsContainer.getComponent<Transform2D>(controlledObj)->flipY();
    };
    auto toggleGravity = [&]() { this->physicsSystem.toggleGravity(); };
    auto toggleLights = [&]() { this->renderer->toggleLight(); };
    auto spawnWizard = [&] {
        ecs::PrefabFactory::createAnimatedSprite(
            ecsContainer, assetsManager.getAtlas("wizard1"), 1, 1);
        std::cout << "Entities: " << ecsContainer.getCurrentEntityCount()
                  << "\n";
    };

    auto& fontAtlas =
        assetsManager.getFontAtlas("test", EngineConstants::Font::defaultFont,
                                   32, EngineConstants::Color::white);
    auto& drawableText = assetsManager.saveDrawableText(
        "drawable", fontAtlas, *renderer,
        assetsManager.getShaderProgram("hudShader"));

    ortoCenter = utils::ortographicProjection(
        -window->getWidthF() / 2, window->getWidthF() / 2,
        -window->getHeightF() / 2, window->getHeightF() / 2);
    ortoGui = utils::ortographicProjection(0, window->getWidthF(),
                                           window->getHeightF(), 0);
    if (onInitCallback) {
        onInitCallback();
    }
}

void Engine2D::run() {
    initializeScene();
    Shader& hudShader = assetsManager.getShaderProgram("hudShader");
    // Shader& hpShader = assetsManager.getShaderProgram("hpShader");
    Shader& batchShader = assetsManager.getShaderProgram("batchShader");
    // Shader& boundingBoxShader =
    // assetsManager.getShaderProgram("boundingBoxShader");
    auto& drawableText = assetsManager.getDrawableText("drawable");

    UniformBuffer ubo(Mat4{}, 3, GL_DYNAMIC_DRAW);
    ubo.bind(1);
    batchShader.bind("Transforms", 1);
    hudShader.bind("Transforms", 1);
    // hpShader.bind("Transforms", 1);
    // boundingBoxShader.bind("Transforms", 1);
    Mat4 projectionView;
    Mat4 ortoGuiCopy;

    auto subscription = healthBarSystem.onDeadEntity(
        [&](ecs::EcsContainer& container, ecs::Entity entity) {
            if (entity == controlledObj) {
                camera.detach();
                controlledObj = {};
            }
            std::cout << "dead " << entity.getId() << '\n';
            ecsContainer.removeEntity(entity);
            std::cout << "Entities: " << ecsContainer.getCurrentEntityCount()
                      << '\n';
        });

    while (!m_quit) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        timeUtils.calcFPS();
        pollEvents();
        if (!paused) {
            aiSystem.update(timeUtils.getDt());
            physicsSystem.update(*collisionSystem, ecsContainer,
                                 timeUtils.getDt());
            collisionSystem->checkCollisions(ecsContainer, timeUtils.getDt());
        }
        if (onUpdateCallback) {
            onUpdateCallback(timeUtils.getDt());
        }
        camera.updatePosition(ecsContainer);
        projectionView = ortoCenter * camera.worldToView();
        ortoGuiCopy = ortoGui;
        ubo.updateStd140(math::transpose(ortoGuiCopy),
                         math::transpose(projectionView));
        // healthBarSystem.update(ecsContainer, *renderer, hpShader);
        spriteSystem.update(ecsContainer, *renderer, batchShader,
                            projectionView, timeUtils.getDt());
        drawableText.render(timeUtils.getFpsText());
        // collisionSystem->renderBoundingBoxes(ecsContainer, *renderer,
        // boundingBoxShader);
        guiSystem.render(*renderer, hudShader, window->getHeightF());
        renderer->batchRender();
        SDL_GL_SwapWindow(window->getWindow());
    }
}