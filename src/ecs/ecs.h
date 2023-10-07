#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include "../logger/logger.h"

const unsigned int MAX_COMPONENTS = 32;

///////////////////////////
// Signature
///////////////////////////
// We use a bitset (1s and 0s) to represent the signature of an entity to
// determine if it has the components required by a system
///////////////////////////

typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int next_id;
};

template <typename T>
class Component: public IComponent {
    public:
        // Returns the unique id of Component<T>
        static int get_id() {
            static auto id = next_id++;
            return id;
        }
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default;
        void Kill();
        int get_id() const;

        // Manage entity tags and groups
        void Tag(const std::string& tag);
        bool HasTag(const std::string& tag) const;
        void Group(const std::string& group);
        bool BelongsToGroup(const std::string& group) const;

        Entity& operator=(const Entity& other) = default;
        bool operator==(const Entity& other) const { return id == other.id; }
        bool operator!=(const Entity& other) const { return id != other.id; }
        bool operator<(const Entity& other) const { return id < other.id; }
        bool operator>(const Entity& other) const { return id > other.id; }

        template <typename TComponent, typename ...TArgs> void add_component(TArgs&& ...args);
        template <typename TComponent> void remove_component();
        template <typename TComponent> bool has_component() const;
        template <typename TComponent> TComponent& get_component() const;

        class Registry* registry;
};
///////////////////////////
// System
///////////////////////////
// The system processes entities that contain a specific signature
///////////////////////////

class System {
    private:
        Signature component_signature;
        std::vector<Entity> entities;
    
    public:
        System() = default;
        ~System() = default;

        void add_entity_to_system(Entity entity);
        void remove_entity_from_system(Entity entity);
        std::vector<Entity> get_system_entities() const;
        const Signature& get_component_signature() const;

        // Defines the component type that entities must have to be considered by the system
        template <typename TComponent> void require_component();
};

///////////////////////////
// Pool
///////////////////////////
// A pool is just a vector (contigious data) of objects of type T
///////////////////////////
class IPool {
    public:
        virtual ~IPool() = default;
        virtual void remove_entity_from_pool(int entity_id) = 0;
};


template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;
        int size;

        std::unordered_map<int, int> entity_id_to_index;
        std::unordered_map<int, int> index_to_entity_id;

    public:
        Pool(int capacity = 100) {
            size = 0;
            data.resize(capacity);
        }
        
        virtual ~Pool() = default;

        bool is_empty() { 
            return data.empty(); 
        }

        int get_size() { 
            return size; 
        }

        void resize(int n) { 
            data.resize(n); 
        }

        void clear() { 
            data.clear(); 
            size = 0;
        }

        void add(T object) { 
            data.push_back(object); 
        }

        void set(int entity_id, T object) { 
            if (entity_id_to_index.find(entity_id) != entity_id_to_index.end()) {
                // if the element already exists, just update it
                int index = entity_id_to_index[entity_id];
                data[index] = object;
            } else {
                int index = size;
                entity_id_to_index.emplace(entity_id, index);
                index_to_entity_id.emplace(index, entity_id);
                if (index >= data.capacity()) {
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
        }

        void remove(int entity_id) { 
            // swap the element to be removed with the last element to keep the data contiguous
            int index_of_removed = entity_id_to_index[entity_id];
            int index_of_last = size - 1;
            data[index_of_removed] = data[index_of_last];

            // update the index-entity_id maps to point to the correct elements
            int entity_id_of_last = index_to_entity_id[index_of_last];
            entity_id_to_index[entity_id_of_last] = index_of_removed;
            index_to_entity_id[index_of_removed] = entity_id_of_last;

            entity_id_to_index.erase(entity_id);
            index_to_entity_id.erase(index_of_last);

            size--;
        }

        void remove_entity_from_pool(int entity_id) override {
            if (entity_id_to_index.find(entity_id) != entity_id_to_index.end()) {
                remove(entity_id);
            }
        }

        T& get(int entity_id) { 
            int index = entity_id_to_index[entity_id];
            return static_cast<T&>(data[index]); 
        }

        T& operator [](unsigned int index) {
            return data[index];
        }

};

///////////////////////////
// Registry
///////////////////////////
// The registry manges the creation and destruction of entities, add systems,
// and add/remove components from entities, etc.
///////////////////////////

class Registry {
    private:
        int num_entities = 0;
        // Vector of component pools, each pool contains all the data for a certain component type.
        // vector index = component type id
        // pool index = entity id
        std::vector<std::shared_ptr<IPool>> component_pools;


        // Vector of component signatures
        // The signature lets us know which components are turned "on" for an entity
        // [vector index = entity id]
        std::vector<Signature> entity_component_signatures;

        // Map of active systems [index = system typeid]
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;    

        // set of entities that are flagged to be added or removed in the next registry update()
        std::set<Entity> entities_to_be_added;
        std::set<Entity> entities_to_be_killed;

        // Entity tags (one tag name per entity)
        std::unordered_map<std::string, Entity> entity_per_tag;
        std::unordered_map<int, std::string> tag_per_entity;

        // Entity groups (a set of entities per group name)
        std::unordered_map<std::string, std::set<Entity>> entities_per_group;
        std::unordered_map<int, std::string> group_per_entity;

        // List of free entity ids
        std::deque<int> free_ids;

    public:
        Registry() { 
            Logger::Log("Registry constructor called!");
        }
        ~Registry() { 
            Logger::Log("Registry destructor called!"); 
        }

        // The registry update() finally processes the entities that are waiting to be added/killed
        void Update();

        // entity management
        Entity create_entity();
        void kill_entity(Entity entity);

        // tag management
        void tag_entity(Entity entity, const std::string& tag);
        bool entity_has_tag(Entity entity, const std::string& tag) const;
        Entity get_entity_by_tag(const std::string& tag) const; 
        void remove_entity_tag(Entity entity);

        // group management
        void group_entity(Entity entity, const std::string& group);
        bool entity_belongs_to_group(Entity entity, const std::string& group) const;
        std::vector<Entity> get_entities_by_group(const std::string& group) const;
        void remove_entity_group(Entity entity);

        // component management
        template <typename TComponent, typename ...TArgs> void add_component(Entity entity, TArgs&& ...args);
        template <typename TComponent> void remove_component(Entity entity);
        template <typename TComponent> bool has_component(Entity entity) const;
        template <typename TComponent> TComponent& get_component(Entity entity) const;

        //system management
        template <typename TSystem, typename ...TArgs> void add_system(TArgs&& ...args);
        template <typename TSystem> void remove_system();
        template <typename TSystem> bool has_system() const;
        template <typename TSystem> TSystem& get_system() const;

        // add and remove entities from systems
        void add_entity_to_systems(Entity entity);
        void remove_entity_from_systems(Entity entity);
};  

template <typename TComponent>
void System::require_component() {
    const auto component_id = Component<TComponent>::get_id();
    component_signature.set(component_id);
}

template <typename TSystem, typename ...TArgs>
void Registry::add_system(TArgs&& ...args) {
    std::shared_ptr<TSystem> new_system = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::remove_system() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::has_system() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::get_system() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Registry::add_component(Entity entity, TArgs&& ...args){
    const auto component_id = Component<TComponent>::get_id();
    const auto entity_id = entity.get_id();

    if (component_id >= component_pools.size()) {
        component_pools.resize(component_id + 1, nullptr);
    }

    if (!component_pools[component_id]) {
        std::shared_ptr<Pool<TComponent>> new_component_pool = std::make_shared<Pool<TComponent>>();
        component_pools[component_id] = new_component_pool;
    }

    std::shared_ptr<Pool<TComponent>> component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);

    TComponent new_component(std::forward<TArgs>(args)...);

    component_pool->set(entity_id, new_component);

    entity_component_signatures[entity_id].set(component_id);
    // if (Game::verbose_logging) {
    //     Logger::Log("Component id = " + std::to_string(component_id) + " added to entity id = " + std::to_string(entity_id));
    // }
}   

template <typename TComponent>
void Registry::remove_component(Entity entity) {
    const auto component_id = Component<TComponent>::get_id();
    const auto entity_id = entity.get_id();

    // remove the component from the component pool
    std::shared_ptr<Pool<TComponent>> component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);
    component_pool->remove(entity_id);

    entity_component_signatures[entity_id].set(component_id, false);
    // if (Game::verbose_logging) {
    //     Logger::Log("Component id = " + std::to_string(component_id) + " removed from entity id = " + std::to_string(entity_id));
    // }
}

template <typename TComponent>
bool Registry::has_component(Entity entity) const {
    const auto component_id = Component<TComponent>::get_id();
    const auto entity_id = entity.get_id();
    return entity_component_signatures[entity_id].test(component_id);
}

template <typename TComponent>
TComponent& Registry::get_component(Entity entity) const {
    const auto component_id = Component<TComponent>::get_id();
    const auto entity_id = entity.get_id();
    auto component_pool = std::static_pointer_cast<Pool<TComponent>>(component_pools[component_id]);
    return component_pool->get(entity_id);
}

template <typename TComponent, typename ...TArgs>
void Entity::add_component(TArgs&& ...args) {
    registry->add_component<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::remove_component() {
    registry->remove_component<TComponent>(*this);
}

template <typename TComponent>
bool Entity::has_component() const {
    return registry->has_component<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::get_component() const {
    return registry->get_component<TComponent>(*this);
}


#endif