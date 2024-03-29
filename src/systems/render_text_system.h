#ifndef RENDER_TEXT_SYSTEM_H
#define RENDER_TEXT_SYSTEM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../ecs/ecs.h"
#include "../components/text_label_component.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"

class RenderTextSystem: public System {
    public:
        RenderTextSystem() {
            require_component<TextLabelComponent>();
        }

        void Render(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store, SDL_Rect& camera) {
            for (auto entity: get_system_entities()) {
                const auto text_label = entity.get_component<TextLabelComponent>();
                const auto transform = entity.get_component<TransformComponent>();

                if (text_label.belongs_to_entity_id != -1) {
                    const auto sprite = entity.get_component<SpriteComponent>();
                    if (sprite.is_hidden) {
                        continue;
                    }
                }

                TTF_Font* font_id = asset_store->get_font(text_label.asset_id);

                SDL_Surface* surface = TTF_RenderText_Blended(font_id, text_label.text.c_str(), text_label.color);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                int label_width = 0;
                int label_height = 0;
                SDL_QueryTexture(texture, NULL, NULL, &label_width, &label_height);

                bool is_text_outside_camera_view = (
                    transform.position.x + (transform.scale.x * label_width) < camera.x ||
                    transform.position.x - (transform.scale.x * label_width) > camera.x + camera.w ||
                    transform.position.y + (transform.scale.y * label_height) < camera.y ||
                    transform.position.y - (transform.scale.y * label_height) > camera.y + camera.h
                );

                if (is_text_outside_camera_view && !text_label.is_fixed) {
                    continue;
                }

                SDL_Rect dst_rect = {
                    static_cast<int>(text_label.position.x - (text_label.is_fixed ? 0 : camera.x)),
                    static_cast<int>(text_label.position.y - (text_label.is_fixed ? 0 : camera.y)),
                    label_width,
                    label_height
                };
                
                SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
                SDL_DestroyTexture(texture);
            }
        }
};

#endif

