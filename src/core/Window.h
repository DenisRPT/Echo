#pragma once
#include <string>

struct GLFWwindow;

class Window{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator = (const Window&) = delete;

    bool shouldClose() const;
    void close();
    void pollEvents();
    void swapBuffers();
    void setTitle(const std::string& title);

    bool keyDown(int glfwKey) const;

    int width() const {return m_width;}
    int height() const { return m_height;}
    float aspect() const {return m_height>0 ? float(m_width)/float(m_height) : 1.0f;}

private:
    static void framebufferSizeCallback(GLFWwindow* window,int w, int h);

    GLFWwindow* m_handle = nullptr;
    int m_width = 0;
    int m_height = 0;
};