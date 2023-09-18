#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

class Game {
    private:
        bool is_running;
        SDL_Window* window;
        SDL_Renderer* renderer;

    public:
        Game();
        ~Game();

        void Initialize();
        void Run();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();

        int window_width;
        int window_height;
};
#endif