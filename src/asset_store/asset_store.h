#ifndef ASSET_STORE_H
#define ASSET_STORE_H

#include <map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class AssetStore {
    private:
        std::map<std::string, SDL_Texture*> textures;
        std::map<std::string, TTF_Font*> fonts;
        // TODO: Create a map for audio

    public:
        AssetStore();
        ~AssetStore();

        // Texture management
        void clear_assets();
        void add_texture(SDL_Renderer* renderer, std::string asset_id, const std::string& file_path, bool get_white);
        SDL_Texture* get_texture(const std::string& asset_id);
        SDL_Surface* ConvertToWhite(SDL_Surface* originalSurface);

        // Font management
        void add_font(const std::string asset_id, const std::string& file_path, int font_size);
        TTF_Font* get_font(const std::string& asset_id);

        // Audio management
        // TODO: Add audio management
        
};

#endif