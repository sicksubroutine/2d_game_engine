#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"
#include "../components/box_collider_component.h"
#include "../components/transform_component.h"

class CollisionSystem: public System {
    public:
        CollisionSystem() {
            require_component<TransformComponent>();
            require_component<BoxColliderComponent>();
        }

        glm::vec3 color; 
        glm::vec3 red = glm::vec3(255.0, 0.0, 0.0);
        glm::vec3 white = glm::vec3(255.0, 255.0, 255.0);

        bool check_collision(glm::vec2(a_pos), double a_width, double a_height, glm::vec2(b_pos), double b_width, double b_height) {
            return (
                a_pos.x < b_pos.x + b_width &&
                a_pos.x + a_width > b_pos.x &&
                a_pos.y < b_pos.y + b_height &&
                a_pos.y + a_height > b_pos.y
            );
        }

        void Update(std::unique_ptr<EventBus>& event_bus, bool is_debug) {
            auto entities = get_system_entities();

            if (is_debug) {
                for (auto entity: entities) {
                    auto& collider = entity.get_component<BoxColliderComponent>();
                    collider.is_colliding = false;
                }
            }
            
            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                auto a_transform = a.get_component<TransformComponent>();
                auto& a_collider = a.get_component<BoxColliderComponent>();

                for (auto j = i; j != entities.end(); j++) {
                    Entity b = *j;

                    if (a == b) {
                        continue;
                    }

                    auto b_transform = b.get_component<TransformComponent>();
                    auto& b_collider = b.get_component<BoxColliderComponent>();

                    if (a_collider.belongs_to_entity_id == b.get_id() || b_collider.belongs_to_entity_id == a.get_id()) {
                        continue;
                    }
                 
                    bool collision_happened = check_collision(
                        a_transform.position + a_collider.offset,
                        a_collider.width * a_transform.scale.x,
                        a_collider.height * a_transform.scale.y,
                        b_transform.position + b_collider.offset,
                        b_collider.width * b_transform.scale.x,
                        b_collider.height * b_transform.scale.y
                    );

                    if (collision_happened) {
                        if (is_debug) {
                            Logger::Log("Entity " + std::to_string(a.get_id()) + " collided with entity " + std::to_string(b.get_id()) + ".");
                        }
                        event_bus->emit_event<CollisionEvent>(a, b);
                    }
                    
                }
            }
        }

        // render bounding boxes, using the red color to indicate collision or white to indicate no collision
        void ColliderDebug(SDL_Renderer* renderer, SDL_Rect& camera) {
            const auto entities = get_system_entities();
            for (auto entity: entities) {
                const auto transform = entity.get_component<TransformComponent>();
                const auto collider = entity.get_component<BoxColliderComponent>();
        
                if (collider.is_colliding) {
                    color = red;
                } else {
                    color = white;
                }

                SDL_Rect collider_rect = {
                    static_cast<int>(transform.position.x + collider.offset.x - camera.x),
                    static_cast<int>(transform.position.y + collider.offset.y - camera.y),
                    static_cast<int>(collider.width * transform.scale.x),
                    static_cast<int>(collider.height * transform.scale.y)
                };
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255.0);
                SDL_RenderDrawRect(renderer, &collider_rect);
            }
        }
};


#endif