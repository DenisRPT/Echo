#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>
#include <iostream>

//checkboard texture for testing
Texture makeCheckerTexture() {
    constexpr int N = 8;
    std::vector<unsigned char> pixels(N * N * 4);
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
			const bool light = ((x + y) % 2 == 0);
            const unsigned char v = light ? 255 : 70;
            const std::size_t i = static_cast<std::size_t>(y * N + x) * 4;
			pixels[i + 0] = v;
			pixels[i + 1] = v;
			pixels[i + 2] = v;
			pixels[i + 3] = 255;
        }
    }
	return Texture(N, N, pixels.data(), Texture::Filter::Nearest);
}


Application::Application()
    : m_window(1280, 720, "Ecou"),
    m_renderer(),
    m_checker(makeCheckerTexture()) {
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
        processInput();

        while(accumulator >= kFixedStep){
            update(static_cast<float>(kFixedStep));
            accumulator -= kFixedStep;
        }

        render();
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
}

void Application::update(float dt){
    m_time += dt;

}

void Application::render() {
    glClearColor(0.01f , 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const float halfH = 10.0f;
	const float halfW = halfH * m_window.aspect();
    const glm::mat4 proj = glm::ortho(-halfW, halfW, -halfH, halfH);

    m_renderer.begin(proj);

    for (int x = 0; x < 50; ++x) {
        for (int y = 0; y < 30; ++y) {
			const float wave = 0.5f + 0.5f * std::sin(m_time * 2.0f + x*0.3f + y*0.2f);
            const glm::vec2 pos((x - 24.5f) * 0.6f, (y - 14.5f) * 0.6f);
            m_renderer.drawQuad(pos, { 0.5f, 0.5f }, { 0.1f,0.3f + 0.6f * wave,1.0f,1.0f });
        }
    }

	m_renderer.drawQuad({ 0.0f, 0.0f }, { 5.0f, 5.0f }, { 1.0f,0.8f,0.3f,0.9f }, m_time, &m_checker);
    m_renderer.end();
}