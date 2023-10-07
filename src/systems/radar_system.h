#ifndef RADAR_SYSTEM_H
#define RADAR_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"
#include "../components/health_component.h"
#include "../utils/utils.h"
#include <SDL2/SDL.h>
#include "../game/game.h"

class RadarSystem: public System {
    // TODO: Implement radar system
    // Basically a mini map that shows the player's position and the position of nearby enemies
    // Reuse fog of war system code to only show enemies that are within a certain radius of the player

    public:
        RadarSystem() {
            require_component<TransformComponent>();
            require_component<SpriteComponent>();
            require_component<HealthComponent>();
        }

        

        void Render(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry) {
            float radar_size = 64.0f; // radar radius from center in pixels
            int radar_starting_x = 10;
            int radar_starting_y = 10;
            float detection_radius = 200.0f;  // radar detection_radius in pixels
            float scale_x = (radar_size / detection_radius) * (Game::map_width / Game::window_width);
            float scale_y = (radar_size / detection_radius) * (Game::map_height / Game::window_height);
            float radar_center_x = radar_size + radar_starting_x;
            float radar_center_y = radar_size + radar_starting_y;

            auto player_group = registry->get_entities_by_group("player");
            if (player_group.empty()) {
                return;
            } else {
                auto entity = player_group[0];
                if (entity.HasTag("player")) {
                    Entity player = entity;
                    const auto player_transform = player.get_component<TransformComponent>();
                    const auto player_position = player_transform.position;

                    // get all the enemies within a detection_radius of the player
                    for (auto entity: get_system_entities()) {
                        const auto transform = entity.get_component<TransformComponent>();
                        const auto entity_position = transform.position;

                        if (Utils::IsWithinCircle(entity_position.x, entity_position.y, player_position.x, player_position.y, static_cast<int>(detection_radius))) {
                            
                            if (entity.HasTag("player")) {
                                continue;
                            }

                            int relative_x = static_cast<int>((entity_position.x - player_position.x) * scale_x);
                            int relative_y = static_cast<int>((entity_position.y - player_position.y) * scale_y);

                            // translate to radar coordinates
                            int dot_x = radar_center_x + relative_x;
                            int dot_y = radar_center_y + relative_y;

                            SDL_Rect rect;
                            int square_size = 4;
                            rect.x = dot_x - square_size / 2;
                            rect.y = dot_y - square_size / 2;
                            rect.w = square_size;
                            rect.h = square_size;

                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &rect);
                        }
                    }
                }
            }
        }
};

#endif 