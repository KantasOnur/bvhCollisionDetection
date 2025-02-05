#pragma once
#include <glm/glm.hpp>
#include "OpenGL.h"
enum EventType
{
	None, 
	WindowResize,
	MouseMove,
	WindowUpdate
};

class Event
{
public:
	virtual EventType getEventType() const { return EventType::None; };
	static EventType getStaticEventType() { return EventType::None; };
};

class WindowResizeEvent : public Event
{
public:
	const int width, height;
	WindowResizeEvent(const int& w, const int& h) : width(w), height(h) {};
	EventType getEventType() const override { return EventType::WindowResize; };
	static EventType getStaticEventType() { return EventType::WindowResize; };
};

class MouseMoveEvent : public Event
{
public:
	const glm::dvec2 mousePos;
	const int width, height;
	GLFWwindow* window;
	MouseMoveEvent(const double& x, const double& y, const int& width, const int& height, GLFWwindow* window)
		: mousePos({ x, y }),
		width(width),
		height(height),
		window(window)
	{};

	EventType getEventType() const override { return EventType::MouseMove; };
	static EventType getStaticEventType() { return EventType::MouseMove; };
};

class WindowUpdateEvent : public Event
{
public:
	GLFWwindow* window;
	WindowUpdateEvent(GLFWwindow* window) : window(window) {};
	EventType getEventType() const override { return EventType::WindowUpdate; };
	static EventType getStaticEventType() { return EventType::WindowUpdate; };
};
