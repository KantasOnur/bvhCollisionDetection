#pragma once
#include <memory>
#include "Window.h"
#include "Mesh.h"
#include "Scene.h"

class Game
{
private:
public:
	static std::unique_ptr<Window> m_window;
	static std::unique_ptr<Scene> m_scene;
public:
	Game();
	void run();
};