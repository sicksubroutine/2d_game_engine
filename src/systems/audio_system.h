#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/audio_component.h"
#include "../asset_store/asset_store.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include "../logger/logger.h"

class AudioSystem : public System {
    public:
        AudioSystem() {
            require_component<AudioComponent>();
        }

        void Update(std::unique_ptr<AssetStore>& asset_store) {
            int current_time = SDL_GetTicks();
            for (auto entity : get_system_entities()) {
                
                auto& audio = entity.get_component<AudioComponent>();
                AudioChannel channel = audio.channel;
                // check if audio is playing and if it is done playing
                if (audio.is_playing) {
                    if (Mix_Playing(channel) == 0) {
                        audio.is_playing = false;
                    } else {
                        continue;
                    }
                }

                double delay = audio.delay * 1000;
                if (audio.asset_id != "") {
                    if (!audio.looping) {
                        continue;
                    }
                    if (current_time - audio.start_time < delay) {
                        continue;
                    } else {
                        audio.start_time = current_time;
                    }
                    
                    Mix_Chunk* sound = asset_store->get_audio(audio.asset_id);
                    
                    if (Mix_PlayChannel(channel, sound, 0) == -1) {
                        Logger::Err("Error playing audio: " + std::string(Mix_GetError()));
                    } else{
                        audio.is_playing = true;
                    }
                }
            }
        }
    

};

#endif
