#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

///////////////////////////
// Signature
///////////////////////////
// We use a bitset to represent the signature of an entity to
// determine if it has the components required by a system
///////////////////////////

typedef std::bitset<MAX_COMPONENTS> Signature;

class Component {
    //TODO:...
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        int GetId() const;
};
///////////////////////////
// System
///////////////////////////
// The system processes entities that contain a specific signature
///////////////////////////

class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;
    
    public:
        System() = default;
        ~System() = default;

        void AddEntitytoSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        Signature& GetComponentSignature() const;
};

class Registry {
    //TODO:...
};

#endif