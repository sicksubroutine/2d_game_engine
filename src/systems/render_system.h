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

        void Render(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera) {
            struct RenderableEntity {
                TransformComponent transform_component;
                SpriteComponent sprite_component;
            };
            
            std::map<SpriteLayer, std::vector<RenderableEntity>> layer_map;

            for (auto entity : get_system_entities()) {
                // Terrible code that will be refactored later
                auto& sprite = entity.get_component<SpriteComponent>();
                if (sprite.hit_flash > 0) {
                        sprite.hit_flash -= 1;
                }
                RenderableEntity renderable_entity;
                renderable_entity.transform_component = entity.get_component<TransformComponent>();
                renderable_entity.sprite_component = entity.get_component<SpriteComponent>();
                layer_map[renderable_entity.sprite_component.layer].emplace_back(renderable_entity);
            }

            for (const auto& [layer, entities] : layer_map) {
                for (const auto& entity : entities) {
                    const auto transform = entity.transform_component;
                    const auto sprite = entity.sprite_component;

                    SDL_Texture* baseTexture = asset_store->get_texture(sprite.asset_id);

                    // use a white texture if the sprite is hit_flashing
                    SDL_Texture* whiteTexture = nullptr;
                    if (sprite.hit_flash > 0) {
                        std::string white_id = sprite.asset_id + "_white";
                        whiteTexture = asset_store->get_texture(white_id);
                    }

                    SDL_Rect src_rect = sprite.src_rect;
                    SDL_Rect dst_rect = {
                        static_cast<int>(transform.position.x - (sprite.is_fixed ? 0 : camera.x)),
                        static_cast<int>(transform.position.y - (sprite.is_fixed ? 0 : camera.y)),
                        static_cast<int>(sprite.width * transform.scale.x),
                        static_cast<int>(sprite.height * transform.scale.y)
                    };
                    SDL_RenderCopyEx(renderer, baseTexture, &src_rect, &dst_rect, transform.rotation, NULL, SDL_FLIP_NONE);

                    if (whiteTexture != nullptr) {
                        int alpha = 175;
                        SDL_SetTextureColorMod(whiteTexture, 255, 128, 128);
                        SDL_SetTextureAlphaMod(whiteTexture, alpha);
                        SDL_RenderCopyEx(renderer, whiteTexture, &src_rect, &dst_rect, transform.rotation, NULL, SDL_FLIP_NONE);
                        SDL_SetTextureAlphaMod(whiteTexture, 255);
        }
    }
}
        
            
            // for (const auto& [layer, entities] : layer_map) {
            //     for (const auto& entity : entities) {
            //         const auto transform = entity.transform_component;
            //         const auto sprite = entity.sprite_component;
            //         SDL_Texture* texture = asset_store->get_texture(sprite.asset_id);
                    
            //         if (sprite.hit_flash > 0) {
            //             std::string white_id = sprite.asset_id + "_white";
            //             texture = asset_store->get_texture(white_id);
            //         }
            

            //         SDL_Rect src_rect = sprite.src_rect;
            //         SDL_Rect dst_rect = {
            //             static_cast<int>(transform.position.x - (sprite.is_fixed ? 0 : camera.x)),
            //             static_cast<int>(transform.position.y - (sprite.is_fixed ? 0 : camera.y)),
            //             static_cast<int>(sprite.width * transform.scale.x),
            //             static_cast<int>(sprite.height * transform.scale.y)
            //         };
            //         SDL_RenderCopyEx(
            //             renderer,
            //             texture,
            //             &src_rect,
            //             &dst_rect,
            //             transform.rotation,
            //             NULL,
            //             SDL_FLIP_NONE
            //         );
            //     }
            // }
        }
};

#endif