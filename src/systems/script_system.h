#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/script_component.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../logger/logger.h"
#include <tuple>

// Declare some native C++ functions that we can call from Lua
std::tuple<double, double> get_entity_position(Entity entity) {
    if (entity.has_component<TransformComponent>()) {
        const auto position = entity.get_component<TransformComponent>().position;
        return std::make_tuple(position.x, position.y);
    } else {
        Logger::Err("Entity does not have a TransformComponent");
    }
    return std::make_tuple(0, 0);
}

void set_entity_position(Entity entity, double x, double y) {
    if (entity.has_component<TransformComponent>()) {
        auto& position = entity.get_component<TransformComponent>().position;
        position.x = x;
        position.y = y;
    } else {
        Logger::Err("Entity does not have a TransformComponent");
    }
}

std::tuple<double, double> get_entity_velocity(Entity entity) {
    if (entity.has_component<RigidBodyComponent>()) {
        const auto velocity = entity.get_component<RigidBodyComponent>().velocity;
        return std::make_tuple(velocity.x, velocity.y);
    } else {
        Logger::Err("Entity does not have a RigidBodyComponent");
    }
    return std::make_tuple(0, 0);
}

void set_entity_velocity(Entity entity, double x, double y) {
    if (entity.has_component<RigidBodyComponent>()) {
        auto& velocity = entity.get_component<RigidBodyComponent>().velocity;
        velocity.x = x;
        velocity.y = y;
    } else {
        Logger::Err("Entity does not have a RigidBodyComponent");
    }
}

double get_entity_rotation(Entity entity) {
    if (entity.has_component<TransformComponent>()) {
        const double rotation = entity.get_component<TransformComponent>().rotation;
        return rotation;
    } else {
        Logger::Err("Entity does not have a TransformComponent");
    }
    return 0;
}

void set_entity_rotation(Entity entity, double angle) {
    if (entity.has_component<TransformComponent>()) {
        auto& rotation = entity.get_component<TransformComponent>().rotation;
        rotation = angle;
    } else {
        Logger::Err("Entity does not have a TransformComponent");
    }
}

void set_projectile_velocity(Entity entity, double x, double y) {
    if (entity.has_component<ProjectileEmitterComponent>()) {
        auto& projectile_vel = entity.get_component<ProjectileEmitterComponent>().projectile_velocity;
        projectile_vel.x = x;
        projectile_vel.y = y;
    } else {
        Logger::Err("Entity does not have a RigidBodyComponent");
    }
}

void set_entity_animation_frame(Entity entity, int frame) {
    if (entity.has_component<AnimationComponent>()) {
        auto& animation = entity.get_component<AnimationComponent>().current_frame;
        animation = frame;
    } else {
        Logger::Err("Entity does not have a AnimationComponent");
    }
}


class ScriptSystem: public System {
    public:
        ScriptSystem() {
            require_component<ScriptComponent>();
        }

        void CreateLuaBinds(sol::state& lua) {
            // create the "entity" usertype so Lua knows what an Entity is
            lua.new_usertype<Entity>(
                "entity",
                "get_id", &Entity::get_id,
                "kill", &Entity::Kill,
                "has_tag", &Entity::HasTag,
                "belongs_to_group", &Entity::BelongsToGroup
            );


            // bind the native C++ functions to Lua
            lua.set_function("get_position", get_entity_position);
            lua.set_function("set_position", set_entity_position);
            lua.set_function("get_velocity", get_entity_velocity);
            lua.set_function("set_velocity", set_entity_velocity);
            lua.set_function("get_rotation", get_entity_rotation);
            lua.set_function("set_rotation", set_entity_rotation);
            lua.set_function("set_projectile_velocity", set_projectile_velocity);
            lua.set_function("set_animation_frame", set_entity_animation_frame);
            
        }

        void Update(double delta_time, int elapsed_time) {
            // call the script function for each entity and invoke their lua function
            for (auto entity: get_system_entities()) {
                const auto script = entity.get_component<ScriptComponent>();
                script.func(entity, delta_time, elapsed_time); // invoke the lua function
            }
        }
};

#endif