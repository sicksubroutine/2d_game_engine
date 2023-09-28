#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

#include "../ecs/ecs.h"
#include "../event_bus/event.h"

class CollisionEvent: public Event {
    public:
        Entity a;
        Entity b;
        CollisionEvent(Entity a, Entity b): a(a), b(b) {}
};

#endif