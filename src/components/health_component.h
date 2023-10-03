#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

struct HealthComponent {
    int current_health;
    int max_health;
    bool is_god_mode;

    HealthComponent(int current_health = 100, int max_health = 100, bool is_god_mode = false) {
        this->current_health = current_health;
        this->max_health = max_health;
        this->is_god_mode = is_god_mode;
    }
};

#endif
