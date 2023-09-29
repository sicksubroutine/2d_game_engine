#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include <string>

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;
    bool is_colliding;
    std::string entity_ignore_id;

    BoxColliderComponent(
        int width = 0, 
        int height = 0, 
        glm::vec2 offset = glm::vec2(0), 
        bool is_colliding = false,
        std::string entity_ignore_id = ""
    ) {
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->is_colliding = is_colliding;
        this->entity_ignore_id = entity_ignore_id;
    }
};

#endif
