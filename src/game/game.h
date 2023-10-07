#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <sol/sol.hpp>
#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"
#include "../components/sprite_component.h"
#include "../event_bus/event_bus.h"

// const int FPS = 60;
// const int MS_PER_FRAME = 1000 / FPS;

class Game {
    private:
        bool is_running;
        int ms_prev_frame = 0;
        float delta_time = 0;
        bool is_debug = false;
        int fps = 0;
        int ms_per_frame = 0;

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect camera;

        sol::state lua;
        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> asset_store;
        std::unique_ptr<EventBus> event_bus;

    public:
        Game();
        ~Game();

        void Initialize();
        void Run();
        void LoadSystems();
        void LuaBindings();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
        void TimeDo();

        static bool verbose_logging;
        static int set_radius;
        static int window_width;
        static int window_height;
        static int map_width;
        static int map_height;
};
#endif