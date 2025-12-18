#include "App.h"

#include <iostream>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

App::App()
    : camera(width, height, glm::vec3(0.0f, 1.0f, 3.0f))
{
    init();
}

App::~App() {
    cleanup();
}

void App::init() {
    // ---------- GLFW ----------
    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        std::exit(-1);
    }

    window = glfwCreateWindow(width, height, "OpenGL App", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    // ---------- GLAD ----------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // ---------- SHADERS ----------
    shader = new Shader(
        "../shaders/basic.vs",
        "../shaders/basic.fs"
    );

    lightShader = new Shader(
        "../shaders/light.vs",
        "../shaders/light.fs"
    );

    shader->Activate();
    glUniform1i(glGetUniformLocation(shader->ID, "texture1"), 0);

    // ---------- GEOMETRY ----------
    CreateGeometry();

    // ---------- TEXTURE ----------
    textureObj = new Texture("../textures/Textura1.png");

    prevTime = glfwGetTime();

    // ---------- LIGHT ----------
    lightPos = glm::vec3(0.2f, -1.5f, 0.2f);
}

void App::CreateGeometry() {

 // Vertices coordinates
    GLfloat vertices[] =
    { //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
    };

    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2, // Bottom side
        0, 2, 3, // Bottom side
        4, 6, 5, // Left side
        7, 9, 8, // Non-facing side
        10, 12, 11, // Right side
        13, 15, 14 // Facing side
    };

    GLfloat lightVertices[] =
    { //     COORDINATES     //
        -0.1f, -0.1f,  0.1f,
        -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f, -0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f,
        -0.1f,  0.1f, -0.1f,
         0.1f,  0.1f, -0.1f,
         0.1f,  0.1f,  0.1f
    };

    GLuint lightIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        3, 7, 6,
        3, 6, 2,
        2, 6, 5,
        2, 5, 1,
        1, 5, 4,
        1, 4, 0,
        4, 5, 6,
        4, 6, 7
    };

    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices, sizeof(vertices));
    ebo = new EBO(indices, sizeof(indices));

    vao->LinkVBO(*vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    vao->LinkVBO(*vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    vao->LinkVBO(*vbo, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    vao->LinkVBO(*vbo, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    vao->Unbind();

    lightIndexCount = sizeof(lightIndices) / sizeof(GLuint);

    lightVAO = new VAO();
    lightVAO->Bind();

    lightVBO = new VBO(lightVertices, sizeof(lightVertices));
    lightEBO = new EBO(lightIndices, sizeof(lightIndices));

    lightVAO->LinkVBO(*lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    lightVAO->Unbind();

    // ---------- COLOR ----------
   

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.2f, 1.5f, 0.2f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
    

    glm::mat4 pyramidModel = glm::mat4(1.0f);

    lightShader->Activate();

    glUniformMatrix4fv(glGetUniformLocation(lightShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    shader->Activate();

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
    glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // ---------- PYRAMID ----------
        shader->Activate();
		glUniform3f(glGetUniformLocation(shader->ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(*shader, "camMatrix");

        textureObj->bind(0);
        vao->Bind();
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
      

        // ---------- LIGHT ----------
        lightShader->Activate();
        camera.Matrix(*lightShader, "camMatrix");
        lightVAO->Bind();
        glDrawElements(GL_TRIANGLES, lightIndexCount, GL_UNSIGNED_INT, 0);
      

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    if (shader) { shader->Delete(); delete shader; }
    if (lightShader) { lightShader->Delete(); delete lightShader; }

    if (ebo) { ebo->Delete(); delete ebo; }
    if (vbo) { vbo->Delete(); delete vbo; }
    if (vao) { vao->Delete(); delete vao; }

    if (lightEBO) { lightEBO->Delete(); delete lightEBO; }
    if (lightVBO) { lightVBO->Delete(); delete lightVBO; }
    if (lightVAO) { lightVAO->Delete(); delete lightVAO; }

    delete textureObj;

    glfwDestroyWindow(window);
    glfwTerminate();
}
