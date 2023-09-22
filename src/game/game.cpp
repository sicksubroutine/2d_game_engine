// #include <SDL2/SDL_ttf.h>
// #include <SDL2/SDL_mixer.h>
// #include <imgui/imgui.h>
// #include <imgui/imgui_sdl.h>
// #include <sol/sol.hpp>
// imports later?

#include "../ecs/ecs.h"
#include "game.h"
#include "../logger/logger.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

Game::Game() {
    is_running = false;
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
    int win_width = FULL_SCREEN ? displayMode.w : WINDOW_WIDTH;
    int win_height = FULL_SCREEN ? displayMode.h : WINDOW_HEIGHT;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_width,
        win_height,
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

void Game::Setup(void) {
    // TODO:
    // Entity player = registry.CreateEntity();
    // tank.AddComponent<TransformComponent>();
    // tank.AddComponent<BoxColliderComponent>();
    // tank.AddComponent<SpriteComponent>("./assets/images/tank.png");
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

    // TODO:
    // MovementSystem.Update();
    // CollisionSystem.Update();
    // DamageSystem.Update();
}

void Game::Render(void) {
    SDL_SetRenderDrawColor(renderer,21,21,21,255);
    SDL_RenderClear(renderer);

    // TODO: Render game objects...

    SDL_RenderPresent(renderer);
}

void Game::Run(void) {
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
