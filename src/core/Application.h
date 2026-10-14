#pragma once
#include "core/Input.h"
#include "core/Window.h"
#include "game/Player.h"
#include "game/TileMap.h"
#include "graphics/Camera2D.h"
#include "graphics/Renderer2D.h"
#include "graphics/Texture.h"
#include <cstdint>

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
    void newWorld(std::uint32_t seed);

    Window m_window;
    Input m_input;
    Renderer2D m_renderer;
    Texture m_glow;
    Camera2D m_camera;
	TileMap m_map;
    Player m_player;

    float m_time = 0.0f;
};