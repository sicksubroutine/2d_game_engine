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
    for (auto texture : textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetStore::add_texture(SDL_Renderer* renderer, std::string asset_id, const std::string& file_path) {
    Logger::Log("Adding texture to asset store with id: " + asset_id);
    SDL_Surface* surface = IMG_Load(file_path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // add the texture to the map
    textures.emplace(asset_id, texture);
}

SDL_Texture* AssetStore::get_texture(const std::string& asset_id) {
    return textures.at(asset_id);
}