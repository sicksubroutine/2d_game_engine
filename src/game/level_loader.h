#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <SDL2/SDL.h>
#include <sol/sol.hpp>
#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"

class LevelLoader {
    public:
        LevelLoader();
        ~LevelLoader();

        void LoadTileMap(const std::unique_ptr<Registry>& registry, std::string asset_id, std::string file_path, int tile_size, int tile_scale);
        void load_level(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& asset_store, SDL_Renderer* renderer, int level_number);
};

#endif