#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"

class MovementSystem: public System {
    public:
        MovementSystem() {
            require_component<TransformComponent>();
            require_component<RigidBodyComponent>();
        }

        void Update(float delta_time) {
            for (auto entity: get_system_entities()) {
                auto& transform = entity.get_component<TransformComponent>();
                const auto& rigid_body = entity.get_component<RigidBodyComponent>();

                transform.position += rigid_body.velocity * delta_time;

                //Logger::Log("Entity " + std::to_string(entity.get_id()) + " position: " + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y));
            }
        }
};

#endif