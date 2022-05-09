#pragma once
#include "Window.h"
#include "ecs/components/Camera2D.h"
#include "TimeUtils.h"
#include "InputHandler.h"
#include "Utils.h"
#include "gui/GuiWindow.h"
#include "gui/GuiButton.h"
#include "AssetsManager.h"
#include "ecs/EcsContainer.h"
#include "ecs/systems/Renderer.h"
#include "ecs/systems/PhysicsSystem.h"
#include "ecs/systems/CollisionSystem2D.h"
#include "ecs/systems/HealthBarSystem.h"
#include "ecs/systems/SpriteSystem.h"
#include "ecs/systems/GuiSystem.h"
#include "ecs/systems/AiSystem.h"
#include "opengl/Shader.h"
#include <memory>
#include <functional>

enum class ShaderType { HUD, HP, DEFAULT };

class Engine2D {
   public:
    using init_fn = std::function<void()>;
    using update_fn = std::function<void(scalar_t)>;
    using window_event_fn = std::function<void(scalar_t, scalar_t)>;

    static Engine2D& getInstance();
    void run();
    void initializeAssets(std::vector<std::string> const& assetsPaths);
    void initializeAssetsFromFileSystem(std::string path);
    ecs::EcsContainer& getEcs();
    AssetsManager& getAssetsManager();
    ecs::PhysicsSystem& getPhysicsSystem();
    Gui::GuiSystem& getGuiSystem();
    ecs::AiSystem& getAiSystem();
    Camera2D& getCamera();
    Renderer& getRenderer();
    Shader& getShader(ShaderType type);
    Window& getWindow();
    InputHandler& getInputHandler();
    void onInit(init_fn fn);
    void onUpdate(update_fn fn);
    void onWindowSizeChange(window_event_fn fn);
    void setPause(bool value);
    void setControlledEntity(ecs::Entity const& entity);
    void setOrto(scalar_t left, scalar_t right, scalar_t bottom, scalar_t top);
    void quit();

   protected:
    void initializeScene();

   private:
    Engine2D();
    ecs::EcsContainer ecsContainer;
    std::unique_ptr<Window> window;
    std::unique_ptr<Gui::GuiWindow> sandboxOptionsWindow;
    std::unique_ptr<Gui::GuiWindow> sandboxOptionsWindow2;
    std::unique_ptr<ecs::CollisionSystem2D> collisionSystem;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Gui::GuiButton> menuButton;
    ecs::SpriteSystem spriteSystem;
    Gui::GuiSystem guiSystem;
    ecs::HealthBarSystem healthBarSystem;
    ecs::AiSystem aiSystem;
    Camera2D camera;
    AssetsManager assetsManager;
    InputHandler inputHandler;
    TimeUtils timeUtils;
    ecs::PhysicsSystem physicsSystem;
    utils::RandomMatrix<scalar_t>& randMatrix =
        utils::RandomMatrix<scalar_t>::instance();
    bool m_quit = false;
    SDL_Event e;
    Mat4 ortoCenter;
    Mat4 ortoGui;
    ecs::Entity controlledObj;
    ecs::Entity cameraAttachedObj;
    init_fn onInitCallback = nullptr;
    update_fn onUpdateCallback = nullptr;
    window_event_fn onWindowSizeChangeCallback = nullptr;
    bool vfxEnabled = true;
    bool vsyncEnabled = true;
    bool paused = false;
    void pollEvents();
};