#include "EventManager.h"

#include <iostream>
#include <cassert>

void EventManager::sub(std::unique_ptr<EventHandlerWrapperInterface>&& handler)
{
    EventType type = handler->getEventType();
    auto listeners = listeners_.find(type);
    if (listeners != listeners_.end())
    {
        auto& callbacks = listeners->second;
        /*
        for (const auto& it : callbacks)
        {
            assert("Attempted to sub identical callbacks", handler->GetType() != it->GetType());
        }
        */
        callbacks.emplace_back(std::move(handler));
    }
    else
    {
        listeners_[type].emplace_back(std::move(handler));
    }
}

void EventManager::triggerEvent(Event& event)
{
    EventType type = event.getEventType();

    for (auto& handlers : listeners_[type])
    {
        handlers->Exec(event);
    }
}