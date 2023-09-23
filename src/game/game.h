#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>


const int WINDOW_WIDTH = 800; 
const int WINDOW_HEIGHT = 600;
const int FPS = 60;
const int MS_PER_FRAME = 1000 / FPS;
const bool FULL_SCREEN = false;

class Game {
    private:
        bool is_running;
        int ms_prev_frame = 0;
        double delta_time = 0;
        SDL_Window* window;
        SDL_Renderer* renderer;

    public:
        Game();
        ~Game();

        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
        void TimeDo();

        int window_width;
        int window_height;
};
#endif