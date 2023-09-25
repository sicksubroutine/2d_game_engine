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
            struct RenderableEntity {
                TransformComponent transform_component;
                SpriteComponent sprite_component;
            };
            
            std::map<SpriteLayer, std::vector<RenderableEntity>> layer_map;

            for (auto entity : get_system_entities()) {
                RenderableEntity renderable_entity;
                renderable_entity.transform_component = entity.get_component<TransformComponent>();
                renderable_entity.sprite_component = entity.get_component<SpriteComponent>();
                layer_map[renderable_entity.sprite_component.layer].emplace_back(renderable_entity);
            }

            for (const auto& [layer, entities] : layer_map) {
                for (const auto& entity : entities) {
                    const auto transform = entity.transform_component;
                    const auto sprite = entity.sprite_component;
                    SDL_Rect src_rect = sprite.src_rect;
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
        }

        // void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store) {
        //     // Create a vector of both sprite and transform components of all entities
        //     struct RenderableEntity {
        //         TransformComponent transform_component;
        //         SpriteComponent sprite_component;
        //     };
        //     std::vector<RenderableEntity> renderable_entities;
        //     for (auto entity: get_system_entities()) {
        //         RenderableEntity renderable_entity;
        //         renderable_entity.transform_component = entity.get_component<TransformComponent>();
        //         renderable_entity.sprite_component = entity.get_component<SpriteComponent>();
        //         renderable_entities.emplace_back(renderable_entity);
        //     }
        //     // Sort the vector by z-index value
        //     std::sort(renderable_entities.begin(), renderable_entities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
        //         return a.sprite_component.z_index < b.sprite_component.z_index;
        //     });

        //     for (auto entity: renderable_entities) {
        //         const auto transform = entity.transform_component;
        //         const auto sprite = entity.sprite_component;

        //         // set the source rectangle
        //         SDL_Rect src_rect = sprite.src_rect;

        //         // set the destination rectangle
        //         SDL_Rect dst_rect = {
        //             static_cast<int>(transform.position.x),
        //             static_cast<int>(transform.position.y),
        //             static_cast<int>(sprite.width * transform.scale.x),
        //             static_cast<int>(sprite.height * transform.scale.y)
        //         };

        //         SDL_RenderCopyEx(
        //             renderer,
        //             asset_store->get_texture(sprite.asset_id),
        //             &src_rect,
        //             &dst_rect,
        //             transform.rotation,
        //             NULL,
        //             SDL_FLIP_NONE
        //         );
                
        //     }
        // }
};

#endif