#ifndef HEALTH_BAR_SYSTEM_H
#define HEALTH_BAR_SYSTEM_H

#include "../ecs/ecs.h"
#include "../utils/utils.h"
#include "../components/transform_component.h"
#include "../components/health_component.h"
#include "../components/text_label_component.h"
#include "../components/sprite_component.h"

class HealthBarSystem: public System {
    public:
        HealthBarSystem() {
            require_component<TransformComponent>();
            require_component<HealthComponent>();
            require_component<TextLabelComponent>();
            require_component<SpriteComponent>();
        }

        void Update() {
            for (auto entity : get_system_entities()) {
                const auto transform = entity.get_component<TransformComponent>();
                const auto health = entity.get_component<HealthComponent>();
                const auto sprite = entity.get_component<SpriteComponent>();
                auto& text_label = entity.get_component<TextLabelComponent>();
                
                float r = Utils::Remap(0, 100, 255, 0, health.health_percentage);
                float g = Utils::Remap(0, 100, 0, 255, health.health_percentage);

                SDL_Color color = {static_cast<Uint8>(r), static_cast<Uint8>(g), 0};

                glm::vec2 health_bar_pos = glm::vec2(transform.position.x + (sprite.width * transform.scale.x), transform.position.y);
                text_label.position = health_bar_pos;
                text_label.text = std::to_string(health.health_percentage);;
                text_label.color = color;
            }
        }
};


#endif
