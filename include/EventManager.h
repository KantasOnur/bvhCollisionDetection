#pragma once

#include <functional>
#include "Events.h"
#include <string>
#include <memory>

template <typename EventT>
using EventHandler = std::function<void(const EventT& event)>;

class EventHandlerWrapperInterface {
public:
    void Exec(const Event& e)
    {
        Call(e);
    }

    virtual std::string GetType() const = 0;
    virtual EventType getEventType() const = 0;

private:
    virtual void Call(const Event& e) = 0;
};


template <typename EventT>
class EventHandlerWrapper : public EventHandlerWrapperInterface {
public:
    explicit EventHandlerWrapper(const EventHandler<EventT>& handler)
        : handler_(handler), handlerType_(handler.target_type().name()) {};

    EventType getEventType() const override { return EventT::getStaticEventType(); };

private:
    void Call(const Event& e) override
    {
        if (e.getEventType() == EventT::getStaticEventType()) {
            handler_(static_cast<const EventT&>(e));
        }
    }

    std::string GetType() const override { return handlerType_; }

    EventHandler<EventT> handler_;
    const std::string handlerType_;
};


class EventManager
{
private:
    std::vector<std::unique_ptr<Event>> queue_;
    std::unordered_map<EventType, std::vector<std::unique_ptr<EventHandlerWrapperInterface>>> listeners_;
    EventManager() = default;
public:
    static EventManager& getInstance()
    {
        static EventManager instance;
        return instance;
    }
    void sub(std::unique_ptr<EventHandlerWrapperInterface>&& handler);
    void triggerEvent(Event& event);

};