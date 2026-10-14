#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>
#include <iostream>

namespace {

    std::uint32_t randomSeed() {
		return static_cast<std::uint32_t>(std::random_device{}());
    }
	//make a soft white circle texture
    Texture makeSoftCircle(int n) {
		std::vector<unsigned char> pixels(static_cast<std::size_t>(n) * n * 4);
        for (int y = 0; y < n; ++y) {
            for (int x = 0; x < n; ++x) {
				const float u = (x + 0.5f) / n * 2.0f - 1.0f;
				const float v = (y + 0.5f) / n * 2.0f - 1.0f;
				float a = std::clamp(1.0f - std::sqrt(u * u + v * v), 0.0f, 1.0f);
                a *= a;
				const std::size_t i = static_cast<std::size_t>(y * n + x) * 4;
                pixels[i + 0] = 255;
                pixels[i + 1] = 255;
                pixels[i + 2] = 255;
				pixels[i + 3] = static_cast<unsigned char>(a * 255.0f + 0.5f);
            }
        }
		return Texture(n, n, pixels.data(), Texture::Filter::Linear);
    }
}


Application::Application()
    : m_window(1280, 720, "Echo"),
    m_renderer(),
    m_glow(makeSoftCircle(64)),
    m_map(96,64,randomSeed()),
    m_player(m_map.spawnPoint()){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	

    m_camera.snapTo(m_map.spawnPoint());
	std::cout << "Map seed: " << m_map.seed() << std::endl;
}

void Application::newWorld(std::uint32_t seed) {
    m_map.generate(seed);
	m_player.teleport(m_map.spawnPoint());
	m_camera.snapTo(m_map.spawnPoint());
	std::cout << "Map seed: " << m_map.seed() << std::endl;
}


void Application::run(){
    constexpr double kFixedStep = 1.0/120.0; //120hz
    constexpr double kMaxFrame = 0.25;

    double previous = glfwGetTime();
    double accumulator = 0.0;
    double fpsTimer = 0.0;
    int frames = 0;

    while(!m_window.shouldClose()){
        const double now = glfwGetTime();
        double frameTime = now - previous;
        previous = now;
        if(frameTime > kMaxFrame) frameTime = kMaxFrame;

        accumulator += frameTime;

        m_window.pollEvents();
		m_input.update(m_window); // once per frame
		processInput(); // one shot input events

        while(accumulator >= kFixedStep){
            update(static_cast<float>(kFixedStep));
            accumulator -= kFixedStep;
        }

		
		const float alpha = static_cast<float>(accumulator / kFixedStep); //interpolation factor for rendering
        render(alpha);
        m_window.swapBuffers();

        ++frames;
        fpsTimer += frameTime;
        if(fpsTimer >= 1.0){
            const auto& st = m_renderer.stats();
            m_window.setTitle("Ecou  |  " + std::to_string(frames) + " FPS  |  " +
                std::to_string(st.quads) + " quads  |  " +
                std::to_string(st.drawCalls) + " draw calls");
            frames = 0;
            fpsTimer = 0.0;
        }
    }
}

void Application::processInput() {
    if(m_window.keyDown(GLFW_KEY_ESCAPE))
        m_window.close();

    if(m_input.pressed(GLFW_KEY_SPACE))
		m_camera.addTrauma(0.6f);

    if (m_input.pressed(GLFW_KEY_R))
        newWorld(randomSeed());
}

void Application::update(float dt){
    m_time += dt;
	m_player.update(dt, m_input,m_map);
    const float impact = m_player.takeImpact();
    if (impact > 3.0f) {
		m_camera.addTrauma(std::min(0.35f,impact* 0.04f));
    }
    m_camera.update(dt, m_player.position());

}

void Application::render(float alpha) {
    glClearColor(0.01f , 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_renderer.begin(m_camera.viewProjection(m_window.aspect(), alpha));

	m_map.draw(m_renderer, m_player.position(alpha), 14.0f);
	m_player.draw(m_renderer, m_glow, alpha, m_time);

    m_renderer.end();
}