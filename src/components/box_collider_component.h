#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include <string>

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;
    bool is_colliding;
    int belongs_to_entity_id;

    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0), bool is_colliding = false, int belongs_to_entity_id = -1) {
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->is_colliding = is_colliding;
        this->belongs_to_entity_id = belongs_to_entity_id;
    }
};

#endif
