#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../logger/logger.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"
#include <SDL2/SDL.h>
#include "../utils/utils.h"

class MovementSystem: public System {
    public:
        MovementSystem() {
            require_component<TransformComponent>();
            require_component<RigidBodyComponent>();
        }

        void subscribe_to_events(const std::unique_ptr<EventBus>& event_bus) {
            event_bus->subscribe_to_event<CollisionEvent>(this, &MovementSystem::on_collision);
        }

        void on_collision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;

            if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                on_enemy_hits_obstacle(a, b);
            } else if (b.BelongsToGroup("enemies") && a.BelongsToGroup("obstacles")) {
                on_enemy_hits_obstacle(b, a);
            }
        }

        void on_enemy_hits_obstacle(Entity enemy, Entity obstacle) {
            if (!enemy.has_component<RigidBodyComponent>() || !enemy.has_component<SpriteComponent>()) {
                return;
            }
            auto& enemy_rigid_body = enemy.get_component<RigidBodyComponent>();
            auto& enemy_sprite = enemy.get_component<SpriteComponent>();

            // check what direction the enemy is moving in, and reverse it
            if (enemy_rigid_body.velocity.x != 0) {
                enemy_rigid_body.velocity.x *= -1;
                enemy_sprite.flip = enemy_rigid_body.velocity.x > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            } 
            if (enemy_rigid_body.velocity.y != 0) {
                enemy_rigid_body.velocity.y *= -1;
                enemy_sprite.flip = enemy_rigid_body.velocity.y > 0 ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
            }
        }

        void Update(float delta_time, int map_width, int map_height) {
            for (auto entity: get_system_entities()) {
                auto& transform = entity.get_component<TransformComponent>();
                const auto sprite = entity.get_component<SpriteComponent>();
                const auto& rigid_body = entity.get_component<RigidBodyComponent>();
                transform.position += rigid_body.velocity * delta_time;

                if (entity.HasTag("player")) {
                    float padding = 5.0f;
                    transform.position.x = Utils::Clamp(static_cast<float>(transform.position.x), padding, static_cast<float>(map_width - sprite.width * transform.scale.x - padding));
                    transform.position.y = Utils::Clamp(static_cast<float>(transform.position.y), padding, static_cast<float>(map_height - sprite.height * transform.scale.y - padding));
                }
            }
        }
};

#endif