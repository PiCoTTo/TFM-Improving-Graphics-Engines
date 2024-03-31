#pragma once

#include <list>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>

#include "Event.h"

// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f

// TODO: redo the system to use std::function instead of pointers?? (Does not seem possible to maintain event type)
// TODO: Improve the system to support inmediate and queue mode

namespace nimo{
class EventDispatcher {
public:
    void Dispatch(const Event& evnt) { //TODO: maybe its needed to move event to raw pointer for queues and multithreading
      Call(evnt);
    }
private:
    virtual void Call(const Event& evnt) = 0;
};

template<class T, class EventType>
class MemberFunctionDispatcher : public EventDispatcher
{
public:
    typedef void (T::*FunctionType)(const EventType&);

    MemberFunctionDispatcher(T* instance, FunctionType memberFunction) : instance{ instance }, memberFunction{ memberFunction } {}

    void Call(const Event& evnt) {
        (instance->*memberFunction)(static_cast<const EventType&>(evnt));
    }
public:
    T* instance;
    FunctionType memberFunction;
};

typedef std::list<EventDispatcher*> DispatcherList;
class EventManager{
public:
    template<typename EventType>
    static void Publish(const EventType& evnt) {
		static_assert(std::is_base_of<Event, EventType>::value, "EventType must derive from Event class");
        DispatcherList* Dispatchers = subscribers[typeid(EventType)];

        if (Dispatchers == nullptr) {
            return;
        }

        for (auto& Dispatcher : *Dispatchers) {
            if (Dispatcher != nullptr) {
                Dispatcher->Dispatch(evnt);
            }
        }
    }

    template<class T, class EventType>
    static void Subscribe(T* instance, void (T::* memberFunction)(const EventType&)) {
		static_assert(std::is_base_of<Event, EventType>::value, "EventType must derive from Event class");
        DispatcherList* Dispatchers = subscribers[typeid(EventType)];

        //First time initialization
        if (Dispatchers == nullptr) {
            Dispatchers = new DispatcherList();
            subscribers[typeid(EventType)] = Dispatchers;
        }

        Dispatchers->push_back(new MemberFunctionDispatcher<T, EventType>(instance, memberFunction));
    }
    
    template <class T, class EventType>
    static void Unsubscribe(T* instance,void(T::* memberFunction)(const EventType&))
    {
		static_assert(std::is_base_of<Event, EventType>::value, "EventType must derive from Event class");
        DispatcherList* Dispatchers = subscribers[typeid(EventType)];
        DispatcherList::iterator i = Dispatchers->begin();
        while (i != Dispatchers->end())
        {
            auto current = static_cast<MemberFunctionDispatcher<T, EventType>*>(*i);
            if (current->memberFunction == memberFunction && current->instance == instance)
            {
                Dispatchers->erase(i);  // Remove from the list
                delete current; // Free memory
                break;
            }
        }
    }
    static void Cleanup()
    {
        for(auto [type, dispatchers] : subscribers)
        {
            for(auto dispatcher : *dispatchers)
            {
                delete dispatcher;
            }
            dispatchers->clear();
            delete dispatchers;
        }
        subscribers.clear();
    }
private:
    static inline std::unordered_map<std::type_index, DispatcherList*> subscribers;
};
};