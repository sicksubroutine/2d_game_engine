#ifndef FOG_OF_WAR_SYSTEM_H
#define FOG_OF_WAR_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/sprite_component.h"
#include "../components/transform_component.h"
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "../utils/utils.h"
#include "../game/game.h"

// System will be in charge of updating the fog of war
// If player is in a certain radius of a tile, that tile will be revealed
// If player is not in a certain radius of a tile, that tile will be hidden upon rendering

class FogOfWarSystem: public System {
    public:
        FogOfWarSystem() {
            require_component<SpriteComponent>();
        }

        bool isWithinCircle(float entity_x, float entity_y, float center_x, float center_y, int radius) {
            float dx = entity_x - center_x;
            float dy = entity_y - center_y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }

        void Update(std::unique_ptr<Registry>& registry) {

            

            // get the player entity
            auto player_group = registry->get_entities_by_group("player");
            if (player_group.empty()) {
                return;
            } else {
                auto entity = player_group[0];
                if (entity.HasTag("player")) {
                    Entity player = entity;
                    const auto player_transform = player.get_component<TransformComponent>();
                    // TODO: Center the circle on the player's position, currently using a upper left corner of the player's position
                    float center_x = player_transform.position.x;
                    float center_y = player_transform.position.y;
                    int radius = Game::set_radius;

                    for (auto entity: get_system_entities()) {
                        // all entities with a sprite component will start off hidden until the player is within a certain radius
                        auto& sprite = entity.get_component<SpriteComponent>();

                        // start each frame with the entity hidden
                        sprite.is_visible = false;
                        // get the entity's transform component
                        const auto transform = entity.get_component<TransformComponent>();

                        // get the entity's position
                        //TODO: Center the circle on the entity's position, currently using a upper left corner of the entity's position
                        float entity_x = transform.position.x;
                        float entity_y = transform.position.y;

                        // if the player is within a certain radius of the entity, then reveal the entity
                        if (Utils::IsWithinCircle(entity_x, entity_y, center_x, center_y, radius)) {
                            sprite.is_hidden = false;
                            sprite.is_revealed = true;
                            sprite.is_visible = true;
                        } else if (sprite.layer != BACKGROUND_LAYER && sprite.layer != GUI_LAYER && sprite.layer != DECORATION_LAYER) {
                            sprite.is_hidden = true;
                        } else if (sprite.layer == GUI_LAYER) {
                            sprite.is_hidden = false;
                            sprite.is_visible = true;
                        }
                    }
                } 
            }
        }
};

#endif