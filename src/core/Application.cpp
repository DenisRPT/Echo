#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

Application::Application()
    : m_window(1280,720,"Echo"), m_shader("shaders/triangle.vert", "shaders/triangle.frag") {
    const float vertices[] = {
    -0.5f, -0.5f,  0.0f, 0.9f, 1.0f,
     0.5f, -0.5f,  0.6f, 0.2f, 1.0f,
     0.0f,  0.5f,  1.0f, 0.2f, 0.6f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Application::~Application() {
    glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
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
            m_window.setTitle("Echo  |  " + std::to_string(frames) + " FPS");
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

}

void Application::render() {
    glClearColor(0.01f , 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.bind();
    m_shader.setFloat("uTime", static_cast<float>(glfwGetTime()));

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}