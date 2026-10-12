#pragma once
#include "core/Input.h"
#include "core/Window.h"
#include "game/Player.h"
#include "graphics/Camera2D.h"
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
    void render(float alpha);

    Window m_window;
    Input m_input;
    Renderer2D m_renderer;
    Texture m_glow;
    Camera2D m_camera;
    Player m_player;

    float m_time = 0.0f;
};