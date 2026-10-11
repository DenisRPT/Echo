#include "core/Window.h";
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(int width, int height, const std::string& title){
    if(!glfwInit())
        throw std::runtime_error("Unable to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title.c_str() , nullptr, nullptr);
    if(!m_handle){
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    glfwMakeContextCurrent(m_handle);

    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){
        glfwDestroyWindow(m_handle);
        glfwTerminate();
        throw std::runtime_error("Unable to load GLAD");
    }

    glfwSwapInterval(1);

    glfwSetWindowUserPointer(m_handle,this);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);

    glfwGetFramebufferSize(m_handle, &m_width, &m_height);
    glViewport(0,0,m_width,m_height);
}

Window::~Window(){
    if(m_handle){
        glfwDestroyWindow(m_handle);
    glfwTerminate();
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
}

void Window::close() {
    glfwSetWindowShouldClose(m_handle,GLFW_TRUE);
}

void Window::pollEvents(){
    glfwPollEvents();
}

void Window::swapBuffers(){
    glfwSwapBuffers(m_handle);
}

void Window::setTitle(const std::string& t){
    glfwSetWindowTitle(m_handle,t.c_str());
}

bool Window::keyDown(int glfwKey)const {
    return glfwGetKey(m_handle,glfwKey) == GLFW_PRESS;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int w, int h){
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_width = w;
    self->m_height = h;
    glViewport(0,0,w,h);
}