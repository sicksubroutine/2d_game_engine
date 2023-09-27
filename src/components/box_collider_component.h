#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;
    bool is_colliding;

    BoxColliderComponent(
        int width = 0, 
        int height = 0, 
        glm::vec2 offset = glm::vec2(0), 
        bool is_colliding = false
    ) {
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->is_colliding = is_colliding;
    }
};

#endif
