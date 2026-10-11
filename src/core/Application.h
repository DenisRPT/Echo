#pragma once
#include "core/Window.h"
#include "graphics/Renderer2D.h"
#include "graphics/Texture.h"

class Application{
public:
    Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void processInput();
    void update(float dt);
    void render();

    Window m_window;
    Renderer2D m_renderer;
	Texture m_checker;

    float m_time = 0.0f;
};