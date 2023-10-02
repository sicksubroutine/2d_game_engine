#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

struct HealthComponent {
    int health_percentage;
    int max_health;
    bool is_god_mode;

    HealthComponent(int health_percentage = 100, int max_health = 100, bool is_god_mode = false) {
        this->health_percentage = health_percentage;
        this->max_health = max_health;
        this->is_god_mode = is_god_mode;
    }
};

#endif
