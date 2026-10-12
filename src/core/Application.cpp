#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

namespace {
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
    m_glow(makeSoftCircle(64)) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
}

void Application::update(float dt){
    m_time += dt;
	m_player.update(dt, m_input);
    m_camera.update(dt, m_player.position());

}

void Application::render(float alpha) {
    glClearColor(0.01f , 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_renderer.begin(m_camera.viewProjection(m_window.aspect(), alpha));

    const glm::vec2 p = m_player.position(alpha);
    constexpr float kSpacing = 2.0f;
    constexpr float kRadius = 14.0f;

	const int minX = static_cast<int>(std::floor((p.x - kRadius) / kSpacing));
	const int maxX = static_cast<int>(std::ceil((p.x + kRadius) / kSpacing));
	const int minY = static_cast<int>(std::floor((p.y - kRadius) / kSpacing));
	const int maxY = static_cast<int>(std::ceil((p.y + kRadius) / kSpacing));

    for (int gx = minX; gx <= maxX; ++gx) {
        for (int gy = minY; gy <= maxY; ++gy) {
			const glm::vec2 pos(gx * kSpacing, gy * kSpacing);
			const float d = glm::length(pos - p);
			if (d >= kRadius) continue;

			float a = 1.0f - d / kRadius;
            a = a * a * 0.8f;
			m_renderer.drawQuad(pos, glm::vec2(0.18f), glm::vec4(0.3f, 0.6f, 1.0f, a), 0.0f, &m_glow);
        }
    }
	m_player.draw(m_renderer, m_glow, alpha, m_time);
    m_renderer.end();
}