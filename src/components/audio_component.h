#ifndef AUDIO_COMPONENT_H
#define AUDIO_COMPONENT_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

struct AudioComponent {

    std::string asset_id;
    bool looping;
    int volume;
    bool is_playing;
    int start_time;
    double delay;

    AudioComponent(std::string asset_id = "", bool looping = false, int volume = 100, double delay = 0.0) {
        this->asset_id = asset_id;
        this->looping = looping;
        this->volume = volume;
        this->is_playing = false;
        this->start_time = SDL_GetTicks();
        this->delay = delay;
    }
    
};

#endif
