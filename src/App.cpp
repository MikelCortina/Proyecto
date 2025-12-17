#include "App.h"

#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <iostream>
#include <cstdlib>

App::App()
    : camera(width, height, glm::vec3(0.0f, 1.0f, 3.0f))
{
    init();
}

App::~App() {
    cleanup();
}

void App::init() {
    // ---------------- GLFW ----------------
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

    // ---------------- GLAD ----------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // ---------------- SHADER ----------------
    shader = new Shader(
        "../shaders/basic.vs",
        "../shaders/basic.fs"
    );

    // 🔑 ENLACE DEL SAMPLER (IMPRESCINDIBLE)
    shader->Activate();
    glUniform1i(glGetUniformLocation(shader->ID, "texture1"), 0);

    // ---------------- GEOMETRÍA ----------------
    CreateGeometry();

    // ---------------- TEXTURA ----------------
    textureObj = new Texture("../textures/Textura1.png");

    prevTime = glfwGetTime();
}

void App::CreateGeometry() {
    GLfloat vertices[] = {
        // pos              // color        // uv
        -0.5f, 0.0f, -0.5f,  1, 0, 0,        0, 0,
         0.5f, 0.0f, -0.5f,  0, 1, 0,        1, 0,
         0.5f, 0.0f,  0.5f,  0, 0, 1,        1, 1,
        -0.5f, 0.0f,  0.5f,  1, 1, 0,        0, 1,
         0.0f, 0.8f,  0.0f,  1, 1, 1,        0.5f, 0.5f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices, sizeof(vertices));
    ebo = new EBO(indices, sizeof(indices));

    vao->LinkVBO(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao->LinkVBO(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao->LinkVBO(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    vao->Unbind();
}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Activate();

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, *shader, "camMatrix");

        textureObj->bind(0);   // 🔑 SLOT 0
        vao->Bind();

        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        vao->Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    if (shader) {
        shader->Delete();
        delete shader;
        shader = nullptr;
    }

    if (ebo) { ebo->Delete(); delete ebo; }
    if (vbo) { vbo->Delete(); delete vbo; }
    if (vao) { vao->Delete(); delete vao; }

    delete textureObj;
    textureObj = nullptr;

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}
