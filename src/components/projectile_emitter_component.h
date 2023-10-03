#ifndef PROJECTILE_EMITTER_COMPONENT_H
#define PROJECTILE_EMITTER_COMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
    glm::vec2 projectile_velocity;
    int repeat_frequency; // in milliseconds
    int projectile_duration; // in milliseconds
    int hit_damage;
    bool is_friendly;
    int last_emission_time; // in milliseconds

    ProjectileEmitterComponent(glm::vec2 projectile_velocity = glm::vec2(0),int repeat_frequency = 0,int projectile_duration = 10000,int hit_damage = 10,bool is_friendly = false) {
        this->projectile_velocity = projectile_velocity;
        this->repeat_frequency = repeat_frequency;
        this->projectile_duration = projectile_duration;
        this->hit_damage = hit_damage;
        this->is_friendly = is_friendly;
        this->last_emission_time = SDL_GetTicks();
    }
};

#endif
