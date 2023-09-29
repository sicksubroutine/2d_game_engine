#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"
#include "../components/box_collider_component.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            require_component<BoxColliderComponent>();
        }

        void subscribe_to_events(std::unique_ptr<EventBus>& event_bus) {
            event_bus->subscribe_to_event<CollisionEvent>(this, &DamageSystem::on_collision);
        }

        void on_collision(CollisionEvent& event) {
            std::string a_id = std::to_string(event.a.get_id());
            std::string b_id = std::to_string(event.b.get_id());


            Logger::Log("Collision event happened between entities " + a_id + " and " + b_id + ".");
            auto& a_collider = event.a.get_component<BoxColliderComponent>();
            auto& b_collider = event.b.get_component<BoxColliderComponent>();

            a_collider.is_colliding = true;
            b_collider.is_colliding = true;

            event.a.Kill();
            event.b.Kill();
        }

        void Update() {
            //TODO
        }
};

#endif
