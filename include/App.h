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
#include "Terrain.h"
#include "Water.h"


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

    GLuint LoadTexture(const std::string& path);

    GLuint LoadCubemap(std::vector<std::string> faces);

    GLFWwindow* window = nullptr;

    const unsigned int width = 800;
    const unsigned int height = 600;

    Camera camera;

    // ---------- SHADERS ----------
    Shader* shader = nullptr;
    Shader* lightShader = nullptr;
    Shader* outliningShader = nullptr;
    Shader* grassShader = nullptr;

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

    Model* crowModel = nullptr;
	Model* crowOutline = nullptr;
    Model* crowModel1 = nullptr;
    Model* crowOutline1 = nullptr;
    Model* crowModel2 = nullptr;
    Model* crowOutline2 = nullptr;
    Model* crowModel3 = nullptr;
    Model* crowOutline3 = nullptr;
    Model* crowModel4 = nullptr;
    Model* crowOutline4 = nullptr;
    Model* crowModel5 = nullptr;
    Model* crowOutline5 = nullptr;
    Model* crowModel6 = nullptr;
    Model* crowOutline6 = nullptr;
    Model* crowModel7 = nullptr;
    Model* crowOutline7 = nullptr;
    Model* crowModel8 = nullptr;
    Model* crowOutline8 = nullptr;




    Terrain* terrain = nullptr;

    float prevTime = 0.0f;

    GLuint grassTextureID = 0;
    GLuint rockTextureID = 0;
	GLuint sandTextureID = 0;

    Water* water = nullptr;
    Shader* waterShader = nullptr;

private:
    Shader* skyboxShader;
    GLuint skyboxVAO, skyboxVBO;
    GLuint skyboxTextureID;  // el cubemap

   
};
