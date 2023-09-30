#ifndef ASSET_STORE_H
#define ASSET_STORE_H

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetStore {
    private:
        std::map<std::string, SDL_Texture*> textures;
        // TODO: Add fonts, sounds, etc.

    public:
        AssetStore();
        ~AssetStore();

        void clear_assets();
        void add_texture(SDL_Renderer* renderer, std::string asset_id, const std::string& file_path, bool get_white);
        SDL_Texture* get_texture(const std::string& asset_id);
        SDL_Surface* ConvertToWhite(SDL_Surface* originalSurface);
        
};

#endif