#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/script_component.h"

class ScriptSystem: public System {
    public:
        ScriptSystem() {
            require_component<ScriptComponent>();
        }

        void Update() {
            // call the script function for each entity and invoke their lua function
            for (auto entity: get_system_entities()) {
                const auto script = entity.get_component<ScriptComponent>();
                script.func(); // invoke the lua function
            }
        }
};

#endif