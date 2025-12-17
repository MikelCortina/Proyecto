
#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Forward declarations
class Texture;
class VAO;
class VBO;
class EBO;

class App {
public:
    App();
    ~App();

    void run();

private:
    void init();
    void CreateGeometry();
    void mainLoop();
    void cleanup();

    std::string loadShaderSource(const std::string& filepath);

    // Estado ventana / GL
    int width = 800;
    int height = 600;
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;

    // Geometría
    VAO* vao = nullptr;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    // Recursos
    Texture* textureObj = nullptr;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    // Animación
    float rotation = 0.0f;
    double prevTime = 0.0;
};
