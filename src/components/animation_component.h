#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <SDL2/SDL.h>

struct AnimationComponent {
    int num_frames;
    int current_frame;
    int frame_rate_speed;
    bool is_looped;
    int start_time;

    AnimationComponent(int num_frames = 1, int frame_rate_speed = 1, bool is_looped = true) {
        this->num_frames = num_frames;
        this->current_frame = 1;
        this->frame_rate_speed = frame_rate_speed;
        this->is_looped = is_looped;
        this->start_time = SDL_GetTicks();
    }
};

#endif
