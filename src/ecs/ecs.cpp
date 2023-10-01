#include "ecs.h"
#include "../logger/logger.h"

int IComponent::next_id = 0;

int Entity::get_id() const {
    return id;
}

void Entity::Kill() {
    registry->kill_entity(*this);
}

void Entity::Tag(const std::string& tag) {
    registry->tag_entity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
    return registry->entity_has_tag(*this, tag);
}

void Entity::Group(const std::string& group) {
    registry->group_entity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const {
    return registry->entity_belongs_to_group(*this, group);
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

    if (free_ids.empty()) {
        // if there are no free ids, create a new one
        entity_id = num_entities++;
        if (entity_id >= entity_component_signatures.size()) {
            entity_component_signatures.resize(entity_id + 1);
        }
    } else {
        entity_id = free_ids.front();
        free_ids.pop_front();
    }

    Entity entity(entity_id);
    entity.registry = this;
    entities_to_be_added.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entity_id));
    return entity;
}

void Registry::kill_entity(Entity entity) {
    entities_to_be_killed.insert(entity);

    Logger::Log("Entity destroyed with id = " + std::to_string(entity.get_id()));
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

void Registry::remove_entity_from_systems(Entity entity) {
    for (auto system: systems) {
        system.second->remove_entity_from_system(entity);
    }
}

void Registry::tag_entity(Entity entity, const std::string& tag) {
    entity_per_tag.emplace(tag, entity);
    tag_per_entity.emplace(entity.get_id(), tag);
}

bool Registry::entity_has_tag(Entity entity, const std::string& tag) const {
    if (tag_per_entity.find(entity.get_id()) == tag_per_entity.end()) {
        return false;
    }
    return entity_per_tag.find(tag)->second == entity;
}

Entity Registry::get_entity_by_tag(const std::string& tag) const {
    return entity_per_tag.at(tag);
}

void Registry::remove_entity_tag(Entity entity) {
    auto tagged_entity = tag_per_entity.find(entity.get_id());
    if (tagged_entity != tag_per_entity.end()) {
        auto tag = tagged_entity->second;
        entity_per_tag.erase(tag);
        tag_per_entity.erase(tagged_entity);
    }
}

void Registry::group_entity(Entity entity, const std::string& group) {
    entities_per_group.emplace(group, std::set<Entity>());
    entities_per_group[group].emplace(entity);
    group_per_entity.emplace(entity.get_id(), group);
}

bool Registry::entity_belongs_to_group(Entity entity, const std::string& group) const {
    if (entities_per_group.find(group) == entities_per_group.end()) {
        return false;
    }
    auto group_entities = entities_per_group.at(group);
    return group_entities.find(entity.get_id()) != group_entities.end();
}

std::vector<Entity> Registry::get_entities_by_group(const std::string& group) const {
    if (entities_per_group.find(group) == entities_per_group.end()) {
        return std::vector<Entity>();
    }
    auto& set_of_entities = entities_per_group.at(group);
    return std::vector<Entity>(set_of_entities.begin(), set_of_entities.end());
}

void Registry::remove_entity_group(Entity entity) {
    // if in group, remove entity from group management
    auto grouped_entity = group_per_entity.find(entity.get_id());
    if (grouped_entity != group_per_entity.end()) {
        auto group = entities_per_group.find(grouped_entity->second);
        if (group != entities_per_group.end()) {
            auto entity_in_group = group->second.find(entity);
            if (entity_in_group != group->second.end()) {
                group->second.erase(entity_in_group);
            }
        }
        group_per_entity.erase(grouped_entity);
    }
}

void Registry::Update() {
    // add entities that are waiting to be added
    for (auto entity: entities_to_be_added) {
        add_entity_to_systems(entity);
    }
    entities_to_be_added.clear();
    // Removing entities from systems that are waiting to be removed
    for (auto entity: entities_to_be_killed){
        remove_entity_from_systems(entity);
        const auto entity_id = entity.get_id();
        entity_component_signatures[entity_id].reset();
        
        // remove the entity from the component pools
        for (auto pool: component_pools) {
            if (pool) {
                pool->remove_entity_from_pool(entity.get_id());
            }
        }

        // make the entity id available to be reused
        free_ids.push_back(entity_id);

        // remove any traces of the entity from the tag/group maps
        remove_entity_tag(entity);
        remove_entity_group(entity);
    }
    entities_to_be_killed.clear();
}