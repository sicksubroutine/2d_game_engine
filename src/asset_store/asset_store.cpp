#include "asset_store.h"
#include "../logger/logger.h"
#include <SDL2/SDL_image.h>

AssetStore::AssetStore() {
    Logger::Log("AssetStore constructor called!");
}

AssetStore::~AssetStore() {
    Logger::Log("AssetStore destructor called!");
    clear_assets();
}

void AssetStore::clear_assets() {
    Logger::Log("Clearing assets...");
    
    // Clear textures
    for (auto texture : textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    // Clear fonts
    for (auto font : fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
}

void AssetStore::add_texture(SDL_Renderer* renderer, std::string asset_id, const std::string& file_path, bool get_white) {
    Logger::Log("Adding texture to asset store with id: " + asset_id);
    SDL_Surface* surface = IMG_Load(file_path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Create a white version of the texture and store it as "asset_id + "_white"
    // This white texture is for a "hit flash" effect when an entity is hit
    if (get_white) {
        SDL_Surface* whiteSurface = ConvertToWhite(surface);
        SDL_Texture* whiteTexture = SDL_CreateTextureFromSurface(renderer, whiteSurface);
        SDL_FreeSurface(whiteSurface);
        textures.emplace(asset_id + "_white", whiteTexture);
    }

    SDL_FreeSurface(surface);
    textures.emplace(asset_id, texture);
}

SDL_Surface* AssetStore::ConvertToWhite(SDL_Surface* originalSurface) {
    SDL_Surface* whiteSurface = SDL_ConvertSurface(originalSurface, originalSurface->format, 0);
    if (!whiteSurface) {
        Logger::Err("Surface conversion failed!");
        return nullptr; // Surface conversion failed.
    }

    Uint32* pixels = (Uint32*)whiteSurface->pixels;
    Uint32 white = SDL_MapRGBA(whiteSurface->format, 255, 255, 255, 255);

    for (int i = 0; i < whiteSurface->w * whiteSurface->h; ++i) {
        Uint32 alpha = pixels[i] & 0xFF000000;
        if (alpha != 0) {  // Skip transparent pixels
            pixels[i] = white | alpha;
        }
    }

    return whiteSurface;
}

void AssetStore::add_font(const std::string asset_id, const std::string& file_path, int font_size) {
    Logger::Log("Adding font to asset store with id: " + asset_id);
    TTF_Font* font = TTF_OpenFont(file_path.c_str(), font_size);
    // if (!font) {
    //     Logger::Err("Failed to load font: " + file_path);
    //     return;
    // }
    fonts.emplace(asset_id, font);
}

TTF_Font* AssetStore::get_font(const std::string& asset_id) {
    if (fonts.find(asset_id) == fonts.end()) {
        Logger::Err("Font not found in asset store with id: " + asset_id);
        return nullptr;
    }
    return fonts.at(asset_id);
}

SDL_Texture* AssetStore::get_texture(const std::string& asset_id) {
    if (textures.find(asset_id) == textures.end()) {
        Logger::Err("Texture not found in asset store with id: " + asset_id);
        return nullptr;
    }
    return textures.at(asset_id);
}