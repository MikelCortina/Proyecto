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
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(0.1f, 0.1, 0.1f, 1.0f);

 
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
	   outliningShader = new Shader("../shaders/outlining.vs", "../shaders/outlining.fs");

      



    // ---------- COLOR ----------
 
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.2f, 1.5f, 0.2f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
    
    shader->Activate();

    glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // Cargar modelo glTF
    model = new Model("../models/crow/scene.gltf");
	model2 = new Model("../models/crow-outline/scene.gltf");
  
   


}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 5000.0f);

		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
		glStencilMask(0xFF); // Write to stencil buffer

        if (model)
            model->Draw(*shader, camera);
	
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // Don't write to stencil buffer
		glDisable(GL_DEPTH_TEST);

		outliningShader->Activate();
		glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);
		if (model2)
			model2->Draw(*outliningShader, camera);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);


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
