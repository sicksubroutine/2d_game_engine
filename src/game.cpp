#include "game.h"
#include <iostream>
#include <SDL2/SDL.h>

Game::Game() {
    is_running = false;
    std::cout << "Game Constructor called..." << std::endl;
}

Game::~Game() {
    std::cout << "Game Destructor called..." << std::endl;
}

void Game::Initialize(void) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING != 0)) {
        std::cerr << "Error initializing SDL." << std::endl;
        return;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(-1, &display_mode);

    window_width = 800; //display_mode.w;
    window_height = 600; //display_mode.h;
    
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        std::cerr << "Error creating SDL window." << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        0
    );

    if (!renderer) {
        std::cerr << "Error creating renderer." << std::endl;
        return;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    is_running = true;

}

void Game::Run(void) {
    while (is_running) {
        ProcessInput();
        Update();
        Render();
    }
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
                break;
        } 
    }

    
}

void Game::Update(void) {

}

void Game::Render(void) {
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    SDL_RenderClear(renderer);

    // TODO: Render all game objects...

    SDL_RenderPresent(renderer);
}

void Game::Destroy(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
