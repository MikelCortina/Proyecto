#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "shaderClass.h"

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

    // Ventana
    int width = 800;
    int height = 600;
    GLFWwindow* window = nullptr;

    // Shader
    Shader* shader = nullptr;

    // Geometría
    VAO* vao = nullptr;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    // Recursos
    Texture* textureObj = nullptr;

    // Cámara
    Camera camera;

    // Tiempo
    double prevTime = 0.0;
};
