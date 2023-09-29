#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

struct HealthComponent {
    int health_percentage;

    HealthComponent(int health_percentage = 100) {
        this->health_percentage = health_percentage;
    }
};

#endif
