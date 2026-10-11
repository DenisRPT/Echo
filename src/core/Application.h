#pragma once
#include "core/Window.h"
#include "graphics/Shader.h"

class Application{
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void processInput();
    void update(float dt);
    void render();

    Window m_window;
    Shader m_shader;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};