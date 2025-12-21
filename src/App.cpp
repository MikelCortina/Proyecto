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
    glDepthFunc(GL_LESS);
    glClearColor(0.85f, 0.85f, 0.90f, 1.0f);

 
    // ---------- GEOMETRY ----------
    CreateGeometry();
    std::cout << "Mesh count: " << model->meshes.size() << "\n";

    prevTime = glfwGetTime();

}


void App::CreateGeometry() {

     Vertex lightVertices[] =
    { //     COORDINATES     //
        Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
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

       // ---------- SHADERS ----------

       shader = new Shader("../shaders/basic.vs", "../shaders/basic.fs");

      



    // ---------- COLOR ----------
 
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.2f, 1.5f, 0.2f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
    
    shader->Activate();

    glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    glEnable(GL_DEPTH_TEST);
    // Cargar modelo glTF
    model = new Model("../models/ground/scene.gltf");
    model2 = new Model("../models/trees/scene.gltf");
   


}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 5000.0f);

        if (model)
            model->Draw(*shader, camera);
		if (model2)
			model2->Draw(*shader, camera);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    if (shader) { shader->Delete(); delete shader; }
    if (lightShader) { lightShader->Delete(); delete lightShader; }


    delete textureObj;

    glfwDestroyWindow(window);
    glfwTerminate();
}
