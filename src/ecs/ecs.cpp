#include "ecs.h"
#include "../logger/logger.h"

int IComponent::next_id = 0;

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
    entity.registry = this;
    entities_to_be_added.insert(entity);
    
    if (entity_id >= entity_component_signatures.size()) {
        entity_component_signatures.resize(entity_id + 1);
    }

    Logger::Log("Entity created with id = " + std::to_string(entity_id));
    return entity;
}

void Registry::add_entity_to_systems(Entity entity) {
    const auto entity_id = entity.get_id();
    
    const auto entity_component_signature = entity_component_signatures[entity_id];
    for (auto& system : systems) {
        const auto& system_component_signature = system.second->get_component_signature();
        bool is_interested = (entity_component_signature & system_component_signature) == system_component_signature;
        if (is_interested) {
            system.second->add_entity_to_system(entity);
        }
    }
}

void Registry::Update() {
    // add entities that are waiting to be added
    for (auto entity: entities_to_be_added) {
        add_entity_to_systems(entity);
    }
    entities_to_be_added.clear();
    // TODO: Removing entities from systems that are waiting to be removed
}