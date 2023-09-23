#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

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
        int get_id() const;

        Entity& operator=(const Entity& other) = default;
        bool operator==(const Entity& other) const { return id == other.id; }
        bool operator!=(const Entity& other) const { return id != other.id; }
        bool operator<(const Entity& other) const { return id < other.id; }
        bool operator>(const Entity& other) const { return id > other.id; }
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
        virtual ~IPool() {}
};


template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;

    public:
        Pool(int size = 100) {
            data.resize(size);
        }
        
        virtual ~Pool() = default;

        bool is_empty() { 
            return data.empty(); 
        }

        int get_size() { 
            return data.size(); 
        }

        void resize(int n) { 
            data.resize(n); 
        }

        void clear() { 
            data.clear(); 
        }

        void add(T object) { 
            data.push_back(object); 
        }

        void set(int index, T object) { 
            data[index] = object; 
        }

        T& get(int index) { 
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
        std::vector<IPool*> component_pools;


        // Vector of component signatures
        // The signature lets us know which components are turned "on" for an entity
        // [vector index = entity id]
        std::vector<Signature> entity_component_signatures;

        // Map of active systems [index = system typeid]
        std::unordered_map<std::type_index, System*> systems;    

        // set of entities that are flagged to be added or removed in the next registry update()
        std::set<Entity> entities_to_be_added;
        std::set<Entity> entities_to_be_killed;

    public:
        Registry() = default;

        // The registry update() finally processes the entities that are waiting to be added/killed
        void Update();

        // entity management
        Entity create_entity();

        // component management
        template <typename TComponent, typename ...TArgs> void add_component(Entity entity, TArgs&& ...args);




        // void add_entity_to_system(Entity entity);

        //
        // Add_Component(Entity entity)
        // get_component(Entity entity)
        //
        // add_system()
        // remove_system()
        // has_system()
        // get_system()



};  

template <typename TComponent>
void System::require_component() {
    const auto component_id = Component<TComponent>::get_id();
    component_signature.set(component_id);
}

template <typename TComponent, typename ...TArgs>
void Registry::add_component(Entity entity, TArgs&& ...args){
    const auto component_id = Component<TComponent>::get_id();
    const auto entity_id = entity.get_id();

    if (component_id >= component_pools.size()) {
        component_pools.resize(component_id + 1, nullptr);
    }

    if (!component_pools[component_id]) {
        Pool<TComponent>* new_component_pool = new Pool<TComponent>();
        component_pools[component_id] = new_component_pool;
    }

    Pool<TComponent>* component_pool = component_pools[component_id];

    if (entity_id >= component_pool->get_size()) {
        component_pool->resize(num_entities);
    }

    TComponent new_component(std::forward<TArgs>(args)...);

    component_pool->set(entity_id, new_component);

    entity_component_signatures[entity_id].set(component_id);
}

#endif