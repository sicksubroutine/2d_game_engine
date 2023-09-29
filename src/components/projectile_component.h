#ifndef PROJECTILE_COMPONENT_H
#define PROJECTILE_COMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent {
    bool is_friendly;
    int hit_percent_damage;
    int duration; // in milliseconds
    int start_time; // in milliseconds

    ProjectileComponent(bool is_friendly = false, int hit_percent_damage = 0, int duration = 0) {
        this->is_friendly = is_friendly;
        this->hit_percent_damage = hit_percent_damage;
        this->duration = duration;
        this->start_time = SDL_GetTicks();
    }
};

#endif
