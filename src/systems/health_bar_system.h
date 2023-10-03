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
                
                int current_health = health.current_health;
                int max_health = health.max_health;
                int health_percentage = std::round(static_cast<float>(current_health) / static_cast<float>(max_health) * 100.0);
                
                float r = Utils::Remap(0, 100, 255, 0, health_percentage);
                float g = Utils::Remap(0, 100, 0, 255, health_percentage);

                SDL_Color color = {static_cast<Uint8>(r), static_cast<Uint8>(g), 0};

                glm::vec2 health_bar_pos = glm::vec2(transform.position.x + (sprite.width * transform.scale.x)+2, transform.position.y);
                text_label.position = health_bar_pos;
                text_label.text = std::to_string(health_percentage);;
                text_label.color = color;
            }
        }
};


#endif
