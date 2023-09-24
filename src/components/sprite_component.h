#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent {
    std::string asset_id;
    int width;
    int height;
    SDL_Rect src_rect;

    SpriteComponent(std::string asset_id ="", int width = 0, int height = 0, int src_rect_x = 0, int src_rect_y = 0) {
        this->asset_id = asset_id;
        this->width = width;
        this->height = height;
        this->src_rect = {src_rect_x, src_rect_y, width, height};
    }
};

#endif