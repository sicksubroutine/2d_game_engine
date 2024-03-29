// systems
#include "../systems/movement_system.h"
#include "../systems/render_system.h"
#include "../systems/animation_system.h"
#include "../systems/collision_system.h"
#include "../systems/damage_system.h"
#include "../systems/keyboard_control_system.h"
#include "../systems/camera_movement_system.h"
#include "../systems/projectile_emit_system.h"
#include "../systems/projectile_lifecycle_system.h"
#include "../systems/render_text_system.h"
#include "../systems/health_bar_system.h"
#include "../systems/render_gui_system.h"
#include "../systems/script_system.h"
#include "../systems/fog_of_war_system.h"
#include "../systems/radar_system.h"
#include "../systems/audio_system.h"

// Others
#include "../utils/utils.h"
#include "../ecs/ecs.h"
#include "game.h"
#include "../logger/logger.h"
#include "level_loader.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <sol/sol.hpp>

int Game::window_width;
int Game::window_height;
int Game::map_width;
int Game::map_height;
int Game::set_radius = 150;
bool Game::verbose_logging;

Game::Game() {
    is_running = false;
    registry = std::make_unique<Registry>();
    asset_store = std::make_unique<AssetStore>();
    event_bus = std::make_unique<EventBus>();
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);
    
    Logger::Log("Game constructor called.");
}

Game::~Game() {
    Logger::Log("Game destructor called.");
}

void Game::Initialize(void) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING != 0)) {
        Logger::Err("Error initializing SDL.");
        return;
    }

    if (TTF_Init() != 0) {
        Logger::Err("Error initializing SDL TTF.");
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        Logger::Err("Error initializing SDL Mixer.");
        return;
    }

    Mix_AllocateChannels(5);

    bool full_screen = false;
    std::string config_file = "./assets/scripts/constants.lua";
    sol::load_result script = lua.load_file(config_file);
    if (!script.valid()) {
        sol::error err = script;
        std::string error_msg = err.what();
        Logger::Err("Error loading config file: " + error_msg);
        return;
    } else {
        lua.script_file(config_file);
        sol::table config = lua["config"];
        window_width = static_cast<int>(config["resolution"]["window_width"]);
        window_height = static_cast<int>(config["resolution"]["window_height"]);
        full_screen = config["full_screen"];
        is_debug = config["debug"];
        fps = config["target_fps"];
        ms_per_frame = 1000 / fps;
        verbose_logging = config["verbose_logging"];
        Logger::debug_to_console = config["debug_to_console"];
    }

    // full screen
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    window_width = full_screen ? displayMode.w : window_width;
    window_height = full_screen ? displayMode.h : window_height;
    if (verbose_logging) {
        Logger::Log("Window width: " + std::to_string(window_width));
        Logger::Log("Window height: " + std::to_string(window_height));
    }
    
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        Logger::Err("Error creating SDL window.");
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        return;
    }

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, window_width, window_height);

    if (full_screen){
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    // initialize some variables
    camera = {0, 0, window_width, window_height};
    is_running = true;

}

void Game::ProcessInput(void) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        // handle ImGui SDL events
        ImGui_ImplSDL2_ProcessEvent(&sdl_event);
        ImGuiIO& io = ImGui::GetIO();

        int mouse_x, mouse_y;
        const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

        io.MousePos = ImVec2(mouse_x, mouse_y);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (sdl_event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }
                // toggle debug mode
                if (sdl_event.key.keysym.sym == SDLK_d && (SDL_GetModState() & KMOD_CTRL)) {
                    is_debug = !is_debug;
                    Logger::Log("Debug mode toggled. Debug mode is now " + std::string(is_debug ? "true" : "false") + ".");
                }
                event_bus->emit_event<KeyPressedEvent>(sdl_event.key.keysym.sym);
                break;
        } 
    }
}

void Game::LoadSystems() {
    registry->add_system<RenderSystem>();
    registry->add_system<RenderTextSystem>();
    registry->add_system<AudioSystem>();
    registry->add_system<MovementSystem>();
    registry->add_system<CollisionSystem>();
    registry->add_system<AnimationSystem>();
    registry->add_system<DamageSystem>();
    registry->add_system<KeyboardControlSystem>();
    registry->add_system<CameraMovementSystem>();
    registry->add_system<ProjectileEmitSystem>();
    registry->add_system<ProjectileLifecycleSystem>();
    registry->add_system<HealthBarSystem>();
    registry->add_system<RenderGUISystem>();
    registry->add_system<ScriptSystem>();
    registry->add_system<FogOfWarSystem>();
    registry->add_system<RadarSystem>();
}

void Game::LuaBindings() {
    registry->get_system<ScriptSystem>().CreateLuaBinds(lua);
}

void Game::Setup() {

    LoadSystems();
    LuaBindings();
    LevelLoader loader;
    loader.load_level(lua, registry, asset_store, renderer, 1);
}

void Game::TimeDo() {
    int time_to_wait = ms_per_frame - (SDL_GetTicks() - ms_prev_frame);
    if (time_to_wait > 0 && time_to_wait <= ms_per_frame) {
        SDL_Delay(time_to_wait);
    }
    delta_time = (SDL_GetTicks() - ms_prev_frame) / 1000.0f;
    ms_prev_frame = SDL_GetTicks();
}

void Game::Update() {
    TimeDo();

    // reset all event handlers for the current frame
    event_bus->reset();

    // subscribe to events
    registry->get_system<DamageSystem>().subscribe_to_events(event_bus);
    registry->get_system<KeyboardControlSystem>().subscribe_to_events(event_bus);
    registry->get_system<ProjectileEmitSystem>().subscribe_to_events(event_bus);
    registry->get_system<MovementSystem>().subscribe_to_events(event_bus);

     // update the registry to process any entities that are waiting to be added/removed
    registry->Update();

    registry->get_system<AudioSystem>().Update(asset_store);
    registry->get_system<FogOfWarSystem>().Update(registry);
    registry->get_system<MovementSystem>().Update(delta_time, map_width, map_height);
    registry->get_system<AnimationSystem>().Update();
    registry->get_system<CollisionSystem>().Update(event_bus, is_debug);
    registry->get_system<ProjectileEmitSystem>().Update(registry);
    registry->get_system<CameraMovementSystem>().Update(camera, map_width, map_height);
    registry->get_system<ProjectileLifecycleSystem>().Update(camera);
    registry->get_system<HealthBarSystem>().Update();
    registry->get_system<ScriptSystem>().Update(delta_time, SDL_GetTicks());

}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer,4,4,4,255);
    SDL_RenderClear(renderer);

    // invoke all of the systems that need to render
    registry->get_system<RenderSystem>().Render(renderer, asset_store, camera);
    registry->get_system<RenderTextSystem>().Render(renderer, asset_store, camera);

    if (is_debug) {
        registry->get_system<CollisionSystem>().ColliderDebug(renderer, camera);
        registry->get_system<RenderGUISystem>().Render(registry, camera, map_width, map_height);
    }
    registry->get_system<RadarSystem>().Render(renderer, registry);

    SDL_RenderPresent(renderer);
}

void Game::Run() {
    if (!window || !renderer) {
        Logger::Err("Error running game.");
        return;
    }
    Setup();
    while (is_running) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}