#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "Camera.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Mesh.h"
#include "Model.h"

class App {
public:
    App();
    ~App();

    void run();

private:
    void init();
    void mainLoop();
    void cleanup();

    void CreateGeometry();

    GLFWwindow* window = nullptr;

    const unsigned int width = 800;
    const unsigned int height = 600;

    Camera camera;

    // ---------- SHADERS ----------
    Shader* shader = nullptr;
    Shader* lightShader = nullptr;

    // ---------- PYRAMID ----------
    VAO* vao = nullptr;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    // ---------- LIGHT ----------
    VAO* lightVAO = nullptr;
    VBO* lightVBO = nullptr;
    EBO* lightEBO = nullptr;
    GLuint lightIndexCount = 0;

    glm::vec3 lightPos;

    // ---------- TEXTURE ----------
    Texture* textureObj = nullptr;
    Texture* textureSpec = nullptr;     // Nueva: specular map (o roughness, etc.)

	Mesh* floor = nullptr;
	Mesh* light = nullptr;

	Model* model = nullptr;
	Model* model2 = nullptr;
    

    float prevTime = 0.0f;
};
