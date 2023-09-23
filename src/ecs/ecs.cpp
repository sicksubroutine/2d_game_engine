#include "ecs.h"
#include "../logger/logger.h"

int Entity::get_id() const {
    return id;
}

void System::add_entity_to_system(Entity entity) {
    entities.push_back(entity);
}
void System::remove_entity_from_system(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) { 
        return entity == other; 
    }), entities.end());
}
std::vector<Entity> System::get_system_entities() const {
    return entities;
}
const Signature& System::get_component_signature() const {
    return component_signature;
}

Entity Registry::create_entity() {
    int entity_id;

    entity_id = num_entities++;

    Entity entity(entity_id);
    entities_to_be_added.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entity_id));
    return entity;
}

void Registry::Update() {
    // TODO: Add the entities that are waiting to be created to the active systems 
}