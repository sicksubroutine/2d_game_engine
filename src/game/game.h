#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"
#include "../components/sprite_component.h"
#include "../event_bus/event_bus.h"

const int WINDOW_WIDTH = 1280; 
const int WINDOW_HEIGHT = 720;
const int FPS = 60;
const int MS_PER_FRAME = 1000 / FPS;
const bool FULL_SCREEN = false;

class Game {
    private:
        bool is_running;
        int ms_prev_frame = 0;
        float delta_time = 0;
        bool is_debug = false;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect camera;
        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> asset_store;
        std::unique_ptr<EventBus> event_bus;

    public:
        Game();
        ~Game();

        void Initialize();
        void Run();
        void LoadLevel(int level_number);
        void LoadSystems();
        void LoadAssets();
        void LoadTileMap(std::string asset_id, std::string file_path, int tile_size, int tile_scale);
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
        void TimeDo();

        static int window_width;
        static int window_height;
        static int map_width;
        static int map_height;
};
#endif