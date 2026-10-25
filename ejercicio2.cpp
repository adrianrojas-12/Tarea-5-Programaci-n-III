#include <iostream>
#include <memory>
using namespace std;

class Window {
public:
    virtual ~Window() {}
    virtual void draw() = 0;
};

class Button {
public:
    virtual ~Button() {}
    virtual void draw() = 0;
};

class Shader {
public:
    virtual ~Shader() {}
    virtual void compile() = 0;
};

class OpenGLWindow : public Window {
public:
    void draw() override { cout << "[OpenGL] Ventana creada con contexto GL" << endl; }
};

class OpenGLButton : public Button {
public:
    void draw() override { cout << "[OpenGL] Boton dibujado con glDrawArrays" << endl; }
};

class OpenGLShader : public Shader {
public:
    void compile() override { cout << "[OpenGL] Shader GLSL compilado con glCompileShader" << endl; }
};

class VulkanWindow : public Window {
public:
    void draw() override { cout << "[Vulkan] Ventana creada con swapchain" << endl; }
};

class VulkanButton : public Button {
public:
    void draw() override { cout << "[Vulkan] Boton dibujado con vkCmdDraw" << endl; }
};

class VulkanShader : public Shader {
public:
    void compile() override { cout << "[Vulkan] Shader compilado a SPIR-V" << endl; }
};

class RenderFactory {
public:
    virtual ~RenderFactory() {}
    virtual unique_ptr<Window> createWindow() = 0;
    virtual unique_ptr<Button> createButton() = 0;
    virtual unique_ptr<Shader> createShader() = 0;
};

class OpenGLFactory : public RenderFactory {
public:
    unique_ptr<Window> createWindow() override { return make_unique<OpenGLWindow>(); }
    unique_ptr<Button> createButton() override { return make_unique<OpenGLButton>(); }
    unique_ptr<Shader> createShader() override { return make_unique<OpenGLShader>(); }
};

class VulkanFactory : public RenderFactory {
public:
    unique_ptr<Window> createWindow() override { return make_unique<VulkanWindow>(); }
    unique_ptr<Button> createButton() override { return make_unique<VulkanButton>(); }
    unique_ptr<Shader> createShader() override { return make_unique<VulkanShader>(); }
};

int main() {
    unique_ptr<RenderFactory> factory = make_unique<OpenGLFactory>();

    auto window = factory->createWindow();
    auto button = factory->createButton();
    window->draw();
    button->draw();

    factory = make_unique<VulkanFactory>();

    auto shader = factory->createShader();
    shader->compile();

    auto window2 = factory->createWindow();
    auto button2 = factory->createButton();
    window2->draw();
    button2->draw();
}