#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"
#include "../asset_store/asset_store.h"
#include <SDL2/SDL.h>

class RenderSystem: public System {
    public:
        RenderSystem() {
            require_component<TransformComponent>();
            require_component<SpriteComponent>();
        }

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store) {
            for (auto entity: get_system_entities()) {
                const auto transform = entity.get_component<TransformComponent>();
                const auto sprite = entity.get_component<SpriteComponent>();

                // set the source rectangle
                SDL_Rect src_rect = sprite.src_rect;

                // set the destination rectangle
                SDL_Rect dst_rect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer,
                    asset_store->get_texture(sprite.asset_id),
                    &src_rect,
                    &dst_rect,
                    transform.rotation,
                    NULL,
                    SDL_FLIP_NONE
                );
                
            }
        }
};

#endif