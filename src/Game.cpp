#include "Game.h"
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include "Gui.h"

std::unique_ptr<Window> Game::m_window;
std::unique_ptr<Scene> Game::m_scene;

Game::Game()
{
    m_window = std::make_unique<Window>(640, 480);
    m_scene = std::make_unique<Scene>(*m_window);
}

void Game::run()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (m_window->isOpen())
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //std::cout << 1 / m_window->getDeltaTime() << std::endl;
        m_scene->render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_window->update();
    }
}