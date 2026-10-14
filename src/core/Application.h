#pragma once
#include "core/Input.h"
#include "core/Window.h"
#include "game/Player.h"
#include "game/Sonar.h"
#include "game/TileMap.h"
#include "graphics/Camera2D.h"
#include "graphics/Renderer2D.h"
#include "graphics/SonarRenderer.h"
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

	static constexpr double kFixedStep = 1.0 / 120.0; // 120 UPS

    Window m_window;
    Input m_input;
    Renderer2D m_renderer;
	SonarRenderer m_sonarRenderer;
    Texture m_glow;
    Camera2D m_camera;
	TileMap m_map;
	Texture m_mapTexture;
    Player m_player;
    Sonar m_sonar;

    float m_time = 0.0f;
    bool m_debugView = false; // F1 shows all map
};