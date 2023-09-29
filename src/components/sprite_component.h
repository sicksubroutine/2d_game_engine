#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <string>
#include <SDL2/SDL.h>

enum SpriteLayer {
    BACKGROUND_LAYER,
    DECORATION_LAYER,
    GROUND_LAYER,
    AIR_LAYER,
    BULLET_LAYER,
    PLAYER_LAYER,
    GUI_LAYER
};

struct SpriteComponent {
    std::string asset_id;
    int width;
    int height;
    SpriteLayer layer;
    bool is_fixed;
    SDL_Rect src_rect;

    SpriteComponent(std::string asset_id ="", int width = 0, int height = 0, SpriteLayer layer = BACKGROUND_LAYER, int src_rect_x = 0, int src_rect_y = 0) {
        this->asset_id = asset_id;
        this->width = width;
        this->height = height;
        this->layer = layer;
        this->is_fixed = layer == GUI_LAYER ? true : false;
        this->src_rect = {src_rect_x, src_rect_y, width, height};
    }
};

// struct SpriteComponent {
//     std::string asset_id;
//     int width;
//     int height;
//     int z_index;
//     SDL_Rect src_rect;

//     SpriteComponent(std::string asset_id ="", int width = 0, int height = 0, int z_index = 0, int src_rect_x = 0, int src_rect_y = 0) {
//         this->asset_id = asset_id;
//         this->width = width;
//         this->height = height;
//         this->z_index = z_index;
//         this->src_rect = {src_rect_x, src_rect_y, width, height};
//     }
// };

#endif
