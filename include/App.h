#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Texture.h" // <--- esto es obligatorio para declarar Texture*
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class App {
public:
    App();
    ~App();
    void run();

private:
    GLFWwindow* window = nullptr;

    void init();
    void mainLoop();
    void cleanup();
    std::string loadShaderSource(const std::string& filepath);
    void CreateVBO();

	const unsigned int width = 800;
	const unsigned int height = 600;

    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    unsigned int VBO = 0, VAO = 0, shaderProgram = 0, EBO = 0;

    Texture* textureObj = nullptr;  // puntero a la textura

	float rotation = 0.0f;
	double prevTime = 0.0f;

};
