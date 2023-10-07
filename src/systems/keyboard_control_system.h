#ifndef KEYBOARD_CONTROL_SYSTEM_H
#define KEYBOARD_CONTROL_SYSTEM_H

#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/key_pressed_event.h"
#include "../components/keyboard_controlled_component.h"
#include "../components/sprite_component.h"
#include "../components/rigid_body_component.h"

class KeyboardControlSystem: public System {
    public:
        KeyboardControlSystem() {
            require_component<KeyboardControlledComponent>();
            require_component<SpriteComponent>();
            require_component<RigidBodyComponent>();
        }

        void subscribe_to_events(std::unique_ptr<EventBus>& event_bus) {
            event_bus->subscribe_to_event<KeyPressedEvent>(this, &KeyboardControlSystem::on_key_pressed);
        }

        void on_key_pressed(KeyPressedEvent& event) {
            // debug
            if (Game::verbose_logging) {
                Logger::Log("Key pressed event emitted: [" + std::to_string(event.symbol) + "] " + std::string(1, event.symbol) + ".");
            }
            for (auto entity: get_system_entities()) {
                const auto keyboard_control = entity.get_component<KeyboardControlledComponent>();
                auto& sprite = entity.get_component<SpriteComponent>();
                auto& rigid_body = entity.get_component<RigidBodyComponent>();

                switch (event.symbol) {
                    case SDLK_UP:
                        rigid_body.velocity = keyboard_control.up_velocity;
                        sprite.src_rect.y = sprite.height * 0;
                        break;
                    case SDLK_RIGHT:
                        rigid_body.velocity = keyboard_control.right_velocity;
                        sprite.src_rect.y = sprite.height * 1;
                        break;
                    case SDLK_DOWN:
                        rigid_body.velocity = keyboard_control.down_velocity;
                        sprite.src_rect.y = sprite.height * 2;
                        break;
                    case SDLK_LEFT:
                        rigid_body.velocity = keyboard_control.left_velocity;
                        sprite.src_rect.y = sprite.height * 3;
                        break;
                }
            }
        }

        void Update() {
            //TODO:
        }

};



#endif
