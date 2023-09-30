#ifndef PROJECTILE_EMIT_SYSTEM_H
#define PROJECTILE_EMIT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/projectile_emitter_component.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/box_collider_component.h"
#include "../components/projectile_component.h"
#include "../components/camera_follow_component.h"
#include <SDL2/SDL.h>

class ProjectileEmitSystem: public System {
    public:
        ProjectileEmitSystem() {
            require_component<ProjectileEmitterComponent>();
            require_component<TransformComponent>();
        }

        void subscribe_to_events(std::unique_ptr<EventBus>& event_bus) {
            // subscribe to the event of the space key being pressed
            event_bus->subscribe_to_event<KeyPressedEvent>(this, &ProjectileEmitSystem::on_key_pressed);
        }

        void on_key_pressed(KeyPressedEvent& event) {
            if (event.symbol == SDLK_SPACE) {
                for (auto entity: get_system_entities()) {
                    if (entity.HasTag("player")) {
                        const auto projectile_emitter = entity.get_component<ProjectileEmitterComponent>();
                        const auto transform = entity.get_component<TransformComponent>();
                        const auto rigid_body = entity.get_component<RigidBodyComponent>();

                        glm::vec2 projectile_position = transform.position;
                        if (entity.has_component<SpriteComponent>()) {
                            auto sprite = entity.get_component<SpriteComponent>();
                            projectile_position.x += (transform.scale.x * sprite.width / 2);
                            projectile_position.y += (transform.scale.y * sprite.height / 2);
                        }

                        glm::vec2 projectile_velocity = projectile_emitter.projectile_velocity;
                        int dir_x = 0;
                        int dir_y = 0;
                        if (rigid_body.velocity.x > 0) dir_x = +1;
                        if (rigid_body.velocity.x < 0) dir_x = -1;
                        if (rigid_body.velocity.y > 0) dir_y = +1;
                        if (rigid_body.velocity.y < 0) dir_y = -1;
                        projectile_velocity.x = projectile_emitter.projectile_velocity.x * dir_x;
                        projectile_velocity.y = projectile_emitter.projectile_velocity.y * dir_y;
                        
                        Entity projectile = entity.registry->create_entity();
                        projectile_do(projectile, projectile_position, projectile_velocity, projectile_emitter.is_friendly, projectile_emitter.hit_percent_damage, projectile_emitter.projectile_duration);
                    }
                }   
            }
        }

        void Update(std::unique_ptr<Registry>& registry) {
            for (auto entity: get_system_entities()) {
                auto& projectile_emitter = entity.get_component<ProjectileEmitterComponent>();
                const auto transform = entity.get_component<TransformComponent>();

                if (projectile_emitter.repeat_frequency == 0) continue;
  
                // check if its time to re-emit a new projectile
                if (SDL_GetTicks() - projectile_emitter.last_emission_time > projectile_emitter.repeat_frequency) {
                    glm::vec2 projectile_position = transform.position;
                    if (entity.has_component<SpriteComponent>()) {
                        const auto sprite = entity.get_component<SpriteComponent>();
                        projectile_position.x += (transform.scale.x * sprite.width / 2);
                        projectile_position.y += (transform.scale.y * sprite.height / 2);
                    }
                    Entity projectile = registry->create_entity();
                    projectile_do(projectile, projectile_position, projectile_emitter.projectile_velocity, projectile_emitter.is_friendly, projectile_emitter.hit_percent_damage, projectile_emitter.projectile_duration);
                    
                    // update the last emission time
                    projectile_emitter.last_emission_time = SDL_GetTicks();
                }
            }
        }

        void projectile_do(Entity projectile, glm::vec2 position, glm::vec2 velocity, bool is_friendly, double hit_percent_damage, int duration) {
            projectile.Group("projectiles");
            projectile.add_component<TransformComponent>(position, glm::vec2(1.0, 1.0), 0.0);
            projectile.add_component<RigidBodyComponent>(velocity);
            projectile.add_component<SpriteComponent>("bullet-image", 4,4, BULLET_LAYER);
            projectile.add_component<BoxColliderComponent>(4, 4, glm::vec2(0), false);
            projectile.add_component<ProjectileComponent>(is_friendly, hit_percent_damage, duration);
        }
};

#endif
