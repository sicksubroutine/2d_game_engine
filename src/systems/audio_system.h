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

            for (auto entity : get_system_entities()) {
                
                auto& audio = entity.get_component<AudioComponent>();
                
                if (audio.asset_id != "") {
                    if (audio.is_playing) {
                        continue;
                    }
                    Mix_Chunk* sound = asset_store->get_audio(audio.asset_id);
                    
                    if (Mix_PlayChannel(-1, sound, audio.looping ? -1 : 0) == -1) {
                        Logger::Err("Error playing audio: " + std::string(Mix_GetError()));
                    } else{
                        audio.is_playing = true;
                    }
                }
            }
        }
    

};

#endif
