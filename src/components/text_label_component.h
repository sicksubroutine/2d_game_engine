#ifndef TEXT_LABEL_COMPONENT_H
#define TEXT_LABEL_COMPONENT_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct TextLabelComponent {
    glm::vec2 position;
    std::string text;
    std::string asset_id;
    SDL_Color color;
    bool is_fixed;
    int belongs_to_entity_id;

    TextLabelComponent(glm::vec2 position = glm::vec2(0), std::string text = "", std::string asset_id = "", SDL_Color color = {0, 0, 0}, bool is_fixed = true, int belongs_to_entity_id = -1) {
        this->position = position;
        this->text = text;
        this->asset_id = asset_id;
        this->color = color;
        this->is_fixed = is_fixed;
        this->belongs_to_entity_id = belongs_to_entity_id;
    }

};

#endif
