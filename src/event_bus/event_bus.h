#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "../logger/logger.h"
#include <map>
#include <typeindex>
#include <list>
#include "event.h"

class IEventCallback {
    private:
        virtual void Call(Event& e) = 0;

    public:
        virtual ~IEventCallback() = default;
        void Execute(Event& e)  {
            Call(e);
        }
};

template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
    private:
        typedef void (TOwner::*CallbackFunction)(TEvent&);

        TOwner* owner_instance;
        CallbackFunction callback_function;

        virtual void Call(Event& e) override {
            std::invoke(callback_function, owner_instance, static_cast<TEvent&>(e));
        }
    public:
        EventCallback(TOwner* owner_instance, CallbackFunction callback_function) {
            this->owner_instance = owner_instance;
            this->callback_function = callback_function;
        }

        virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
    private:
        std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

    public:
        EventBus() {
            Logger::Log("EventBus created.");
        }
        ~EventBus() {
            Logger::Log("EventBus destroyed.");
        }

        // clears the subscribers list
        void reset() {
            subscribers.clear();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // subscribe to event type <T>
        // in our implementation, a listener subscribes to an event type by providing a callback function
        // the callback function is a member function of the listener
        // Example EventBus->SubscribeToEvent<CollisionEvent>(this, &Game::OnCollision);
        /////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename TEvent, typename TOwner>
        void subscribe_to_event(TOwner* owner_instance, void (TOwner::*callback_function)(TEvent&)) {
            if (!subscribers[typeid(TEvent)].get()) {
                subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
            }
            auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(owner_instance, callback_function);
            subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // emit event type <T>
        // in our implementation, an event is emitted by calling the emit_event function
        // the emit_event function is a member function of the event
        // Example EventBus->EmitEvent<CollisionEvent>(player, enemy);
        /////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename TEvent, typename... TArgs>
        void emit_event(TArgs&&... args) {
            auto handlers = subscribers[typeid(TEvent)].get();
            if (handlers) {
                for (auto it = handlers->begin(); it != handlers->end(); ++it) {
                    auto handler = it->get();
                    TEvent event(std::forward<TArgs>(args)...);
                    handler->Execute(event);
                }
            }
        }
};

#endif

