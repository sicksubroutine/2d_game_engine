#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/animation_component.h"
#include "../components/sprite_component.h"
#include <SDL2/SDL.h>

class AnimationSystem: public System {
    public:
        AnimationSystem() {
            require_component<SpriteComponent>();
            require_component<AnimationComponent>();
        }

        void Update() {
            for (auto entity: get_system_entities()){
                auto& animation = entity.get_component<AnimationComponent>();
                auto& sprite = entity.get_component<SpriteComponent>();
                if (animation.is_looped) {
                    animation.current_frame = ((SDL_GetTicks() - animation.start_time) * animation.frame_rate_speed / 1000) % animation.num_frames;
                } else {
                    if (animation.current_frame == animation.num_frames - 1) {
                        animation.current_frame = animation.num_frames - 1;
                    } else {
                        animation.current_frame = ((SDL_GetTicks() - animation.start_time) * animation.frame_rate_speed / 1000) % animation.num_frames;
                    }
                }
                sprite.src_rect.x = sprite.width * animation.current_frame;        
            }
        }
};

#endif