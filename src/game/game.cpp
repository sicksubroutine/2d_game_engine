// #include <SDL2/SDL_ttf.h>
// #include <SDL2/SDL_mixer.h>
// #include <imgui/imgui.h>
// #include <imgui/imgui_sdl.h>
// #include <sol/sol.hpp>
// imports later?

#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/animation_component.h"
#include "../systems/movement_system.h"
#include "../systems/render_system.h"
#include "../systems/animation_system.h"
#include "../ecs/ecs.h"
#include "game.h"
#include "../logger/logger.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

Game::Game() {
    is_running = false;
    registry = std::make_unique<Registry>();
    asset_store = std::make_unique<AssetStore>();
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

    // full screen
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
   //int win_width = FULL_SCREEN ? displayMode.w : WINDOW_WIDTH;
   //int win_height = FULL_SCREEN ? displayMode.h : WINDOW_HEIGHT;

    // Logger::Log("Window width: " + std::to_string(win_width));
    // Logger::Log("Window height: " + std::to_string(win_height));
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
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

    if (FULL_SCREEN){
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    is_running = true;

}

void Game::ProcessInput(void) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {

        switch (sdl_event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (sdl_event.key.keysym.sym == SDLK_ESCAPE)
                    is_running = false;
                if (sdl_event.key.keysym.sym == SDLK_w)
                    
                if (sdl_event.key.keysym.sym == SDLK_s)
                    
                if (sdl_event.key.keysym.sym == SDLK_a)
                    
                if (sdl_event.key.keysym.sym == SDLK_d)
                    
                break;
            case SDL_KEYUP:
                if (sdl_event.key.keysym.sym == SDLK_w)
                    
                if (sdl_event.key.keysym.sym == SDLK_s)
                    
                if (sdl_event.key.keysym.sym == SDLK_a)
                    
                if (sdl_event.key.keysym.sym == SDLK_d)
                    
                break;
        } 
    }
}

void Game::LoadTileMap(std::string asset_id, std::string file_path, int tile_size, int tile_scale) {
    std::vector<std::vector<int>> tile_data;

    // Read file and store data in tile_data
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
    int scale = tile_size * tile_scale;

    // Iterate through tile_data to create entities and components
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

            // Create entity and add components
            Entity tile_entity = registry->create_entity();
            tile_entity.add_component<TransformComponent>(glm::vec2(x * (scale), y * (scale)), glm::vec2(tile_scale, tile_scale), 0.0);
            tile_entity.add_component<SpriteComponent>(asset_id, tile_size, tile_size, BACKGROUND_LAYER, src_rect.x, src_rect.y);
        }
    }
}

void Game::LoadLevel(int level_number) {
    // add the systems that need to be processed in our game
    registry->add_system<MovementSystem>();
    registry->add_system<RenderSystem>();
    registry->add_system<AnimationSystem>();

    // Adding assets to the asset store
    asset_store->add_texture(renderer, "tilemap", "./assets/tilemaps/jungle.png");
    asset_store->add_texture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    asset_store->add_texture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    asset_store->add_texture(renderer, "chopper-image", "./assets/images/chopper.png");
    asset_store->add_texture(renderer, "radar-image", "./assets/images/radar.png");
    
    // loading the tilemap
    LoadTileMap("tilemap", "./assets/tilemaps/jungle.map", 32, 2.0);

    //create some entities
    Entity chopper = registry->create_entity();
    chopper.add_component<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(2.0, 2.0), 0.0);
    chopper.add_component<RigidBodyComponent>(glm::vec2(10.0, 0.0));
    chopper.add_component<SpriteComponent>("chopper-image", 32, 32, PLAYER_LAYER);
    chopper.add_component<AnimationComponent>(2, 15, true);

    Entity radar = registry->create_entity();
    radar.add_component<TransformComponent>(glm::vec2(WINDOW_WIDTH-74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.add_component<SpriteComponent>("radar-image", 64, 64, GUI_LAYER);
    radar.add_component<AnimationComponent>(8, 5, true);

    // add some components to the tank
    Entity tank = registry->create_entity();
    tank.add_component<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(2.0, 2.0), 0.0);
    tank.add_component<RigidBodyComponent>(glm::vec2(5.0, 0.0));
    tank.add_component<SpriteComponent>("tank-image", 32, 32, GROUND_LAYER);

    // // add some components to the truck
    Entity truck = registry->create_entity();
    truck.add_component<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(2.0, 2.0), 45.0);
    truck.add_component<RigidBodyComponent>(glm::vec2(25.0, 25.0));
    truck.add_component<SpriteComponent>("truck-image", 32, 32, AIR_LAYER);
}

void Game::Setup(void) {
    
    LoadLevel(1);
}

void Game::TimeDo(void) {
    int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - ms_prev_frame);
    if (time_to_wait > 0 && time_to_wait <= MS_PER_FRAME) {
        SDL_Delay(time_to_wait);
    }
    delta_time = (SDL_GetTicks() - ms_prev_frame) / 1000.0f;
    ms_prev_frame = SDL_GetTicks();
}

void Game::Update(void) {
    TimeDo();

    registry->get_system<MovementSystem>().Update(delta_time);
    registry->get_system<AnimationSystem>().Update();

    // update the registry to process any entities that are waiting to be added/removed
    registry->Update();

    // registry->get_system<CollisionSystem>().Update();
    // DamageSystem.Update();
}

void Game::Render(void) {
    SDL_SetRenderDrawColor(renderer,21,21,21,255);
    SDL_RenderClear(renderer);

    // invoke all of the systems that need to render
    registry->get_system<RenderSystem>().Update(renderer, asset_store);

    SDL_RenderPresent(renderer);
}

void Game::Run(void) {
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

void Game::Destroy(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}