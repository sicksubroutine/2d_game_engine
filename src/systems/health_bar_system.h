#ifndef HEALTH_BAR_SYSTEM_H
#define HEALTH_BAR_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/health_component.h"
#include "../components/text_label_component.h"

class HealthBarSystem: public System {
    public:
        HealthBarSystem() {
            require_component<TransformComponent>();
            require_component<HealthComponent>();
            require_component<TextLabelComponent>();
        }

        float Lerp(float a, float b, float f) {
            return a + f * (b - a);
        }

        float InvLerp(float a, float b, float v) {
            return (v - a) / (b - a);
        }

        float Remap(float a, float b, float c, float d, float v) {
            float t = InvLerp(a, b, v);
            return Lerp(c, d, t);
        }

        void Update() {
            for (auto entity : get_system_entities()) {
                const auto transform = entity.get_component<TransformComponent>();
                const auto health = entity.get_component<HealthComponent>();
                auto& text_label = entity.get_component<TextLabelComponent>();
                

                SDL_Color color = {0, 255, 0};
                if (health.health_percentage < 30) {
                    color = {255, 0, 0};
                } else if (health.health_percentage < 60) {
                    color = {255, 255, 0};
                }

                glm::vec2 position = transform.position;
                glm::vec2 offset = glm::vec2(10, -10);
                text_label.position = position + offset;
                text_label.text = std::to_string(health.health_percentage);;
                text_label.color = color;
            }
        }
};


#endif
