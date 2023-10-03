// #include <SDL2/SDL_ttf.h>
// #include <SDL2/SDL_mixer.h>
// #include <imgui/imgui.h>
// #include <imgui/imgui_sdl.h>
// #include <sol/sol.hpp>
// imports later?

// components
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/box_collider_component.h"
#include "../components/sprite_component.h"
#include "../components/animation_component.h"
#include "../components/keyboard_controlled_component.h"
#include "../components/camera_follow_component.h"
#include "../components/projectile_emitter_component.h"
#include "../components/health_component.h"
#include "../components/text_label_component.h"

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

// Others
#include "../utils/utils.h"
#include "../ecs/ecs.h"
#include "game.h"
#include "../logger/logger.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <fstream>
#include <sstream>

int Game::window_width;
int Game::window_height;
int Game::map_width;
int Game::map_height;

Game::Game() {
    is_running = false;
    registry = std::make_unique<Registry>();
    asset_store = std::make_unique<AssetStore>();
    event_bus = std::make_unique<EventBus>();
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

    // full screen
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    window_width = FULL_SCREEN ? displayMode.w : WINDOW_WIDTH;
    window_height = FULL_SCREEN ? displayMode.h : WINDOW_HEIGHT;

    //Logger::Log("Window width: " + std::to_string(window_width));
    //Logger::Log("Window height: " + std::to_string(window_height));
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

    if (FULL_SCREEN){
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

void Game::LoadTileMap(std::string asset_id, std::string file_path, int tile_size, int tile_scale) {
    std::vector<std::vector<int>> tile_data;
    int scale = tile_size * tile_scale;

    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream s(line);
        std::string field;
        while (getline(s, field, ',')) {
            row.push_back(std::stoi(field));
        }
        tile_data.push_back(row);
    }

    map_width = tile_data[0].size() * scale;
    map_height = tile_data.size() * scale;
    
    for (int y = 0; y < tile_data.size(); ++y) {
        for (int x = 0; x < tile_data[y].size(); ++x) {
            int tile_id = tile_data[y][x]; 

            SDL_Rect src_rect;
            int tile_row = tile_id / 10; 
            int tile_col = tile_id % 10;

            src_rect.x = tile_col * tile_size;
            src_rect.y = tile_row * tile_size;
            src_rect.w = tile_size;
            src_rect.h = tile_size;

            Entity tile_entity = registry->create_entity();
            tile_entity.Group("tiles");
            tile_entity.add_component<TransformComponent>(glm::vec2(x * (scale), y * (scale)), glm::vec2(tile_scale, tile_scale), 0.0);
            tile_entity.add_component<SpriteComponent>(asset_id, tile_size, tile_size, BACKGROUND_LAYER, src_rect.x, src_rect.y);
        }
    }
}

void Game::LoadSystems() {
    // add the systems that need to be processed in our game
    registry->add_system<RenderSystem>();
    registry->add_system<RenderTextSystem>();
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
}

void Game::LoadAssets() {
    // Add fonts to the asset store
    asset_store->add_font("charriot-font", "./assets/fonts/charriot.ttf", 16);
    //asset_store->add_font("arial-font", "./assets/fonts/arial.ttf", 16);
    asset_store->add_font("pico8-font-10", "./assets/fonts/pico8.ttf", 10);
    asset_store->add_font("pico8-font-7", "./assets/fonts/pico8.ttf", 7);

    // Adding assets to the asset store
    asset_store->add_texture(renderer, "tilemap", "./assets/tilemaps/jungle.png", false);
    asset_store->add_texture(renderer, "tank-image", "./assets/images/tank-panther-right.png", true);
    asset_store->add_texture(renderer, "truck-image", "./assets/images/truck-ford-right.png", true);
    asset_store->add_texture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png", true);
    asset_store->add_texture(renderer, "radar-image", "./assets/images/radar.png", true);
    asset_store->add_texture(renderer, "bullet-image", "./assets/images/bullet.png", true);
    
    // loading the tilemap
    LoadTileMap("tilemap", "./assets/tilemaps/jungle.map", 32, 2.0);
}

void Game::LoadLevel(int level_number) {
    
    LoadAssets();
    LoadSystems();
    
    Entity label = registry->create_entity();
    label.Group("GUI");
    label.add_component<TextLabelComponent>(glm::vec2(window_width/2-40, 10), "CHOPPER 1.0", "pico8-font-10", col.green, true);

    // create the radar entity
    Entity radar = registry->create_entity();
    radar.Group("GUI");
    radar.add_component<TransformComponent>(glm::vec2(WINDOW_WIDTH-74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.add_component<SpriteComponent>("radar-image", 64, 64, GUI_LAYER);
    radar.add_component<AnimationComponent>(8, 5, true);

    //create some entities
    Entity chopper = registry->create_entity();
    chopper.Tag("player");
    chopper.Group("player");
    chopper.add_component<TransformComponent>(glm::vec2(0.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.add_component<SpriteComponent>("chopper-image", 32, 32, PLAYER_LAYER);
    chopper.add_component<AnimationComponent>(2, 15, true);
    chopper.add_component<BoxColliderComponent>(24, 24, glm::vec2(4.0, 4.0));
    chopper.add_component<KeyboardControlledComponent>(glm::vec2(0.0, -80.0),glm::vec2(80.0, 0.0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
    chopper.add_component<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
    chopper.add_component<CameraFollowComponent>();
    chopper.add_component<HealthComponent>(100, 100, false);
    chopper.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);


    // add tank entity
    Entity tank = registry->create_entity();
    tank.Group("enemies");
    tank.add_component<TransformComponent>(glm::vec2(500.0, 600.0), glm::vec2(2.0, 2.0), 0.0);
    tank.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    tank.add_component<SpriteComponent>("tank-image", 32, 32, GROUND_LAYER);
    tank.add_component<BoxColliderComponent>(22, 22, glm::vec2(10.0, 10.0));
    tank.add_component<ProjectileEmitterComponent>(glm::vec2(0.0, -100.0), 5000, 10000, 25, false);
    tank.add_component<HealthComponent>(100, 100, false);
    tank.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);

    // add truck entity
    Entity truck = registry->create_entity();
    truck.Group("enemies");
    truck.add_component<TransformComponent>(glm::vec2(100.0, 475.0), glm::vec2(2.0, 2.0), 0.0);
    truck.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    truck.add_component<SpriteComponent>("truck-image", 32, 32, GROUND_LAYER);
    truck.add_component<BoxColliderComponent>(22, 22, glm::vec2(10.0, 10.0));
    truck.add_component<ProjectileEmitterComponent>(glm::vec2(0.0, -100.0), 1000, 5000, 10, false);
    truck.add_component<HealthComponent>(50, 100, false);
    truck.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
}

void Game::Setup() {
    LoadLevel(1);
}

void Game::TimeDo() {
    int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - ms_prev_frame);
    if (time_to_wait > 0 && time_to_wait <= MS_PER_FRAME) {
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
     // update the registry to process any entities that are waiting to be added/removed
    registry->Update();

    registry->get_system<MovementSystem>().Update(delta_time);
    registry->get_system<AnimationSystem>().Update();
    registry->get_system<CollisionSystem>().Update(event_bus, is_debug);
    registry->get_system<ProjectileEmitSystem>().Update(registry);
    registry->get_system<CameraMovementSystem>().Update(camera, map_width, map_height);
    registry->get_system<ProjectileLifecycleSystem>().Update(camera);
    registry->get_system<HealthBarSystem>().Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer,21,21,21,255);
    SDL_RenderClear(renderer);

    // invoke all of the systems that need to render
    registry->get_system<RenderSystem>().Render(renderer, asset_store, camera);
    registry->get_system<RenderTextSystem>().Render(renderer, asset_store, camera);
    // a system that renders the bounding boxes of the colliders for debugging

    
    if (is_debug) {
        registry->get_system<CollisionSystem>().ColliderDebug(renderer, camera);
        registry->get_system<RenderGUISystem>().Render(registry, camera);
    }

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