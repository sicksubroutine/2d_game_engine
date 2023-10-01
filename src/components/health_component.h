#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

struct HealthComponent {
    int health_percentage;
    bool is_god_mode;

    HealthComponent(int health_percentage = 100, bool is_god_mode = false) {
        this->health_percentage = health_percentage;
    }
};

#endif
