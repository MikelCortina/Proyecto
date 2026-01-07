#include "App.h"
#include <iostream>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

App::App()
    : camera(width, height, glm::vec3(0.0f, 5.0f, 10.0f))  // Cámara más alejada y arriba para ver mejor el terreno centrado
{
    init();
}

App::~App() {
    cleanup();
}

GLuint App::LoadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        stbi_set_flip_vertically_on_load(false); // normalmente no flip para skybox
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Error cargando cubemap: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void App::init() {
    // ---------- GLFW ----------
    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        std::exit(-1);
    }

    // Pedimos OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Terreno OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // ---------- GLAD ----------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f); // Fondo azul oscuro para ver mejor si no hay terreno

    // ---------- GEOMETRY ----------
    CreateGeometry();
    prevTime = glfwGetTime();
}

void App::CreateGeometry() {

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Después de cargar otras cosas...
    std::vector<std::string> faces = {
        "../textures/skybox/right.jpg",   // +X
        "../textures/skybox/left.jpg",    // -X
        "../textures/skybox/top.jpg",     // +Y
        "../textures/skybox/bottom.jpg",  // -Y
        "../textures/skybox/front.jpg",   // +Z
        "../textures/skybox/back.jpg"     // -Z
    };
    skyboxTextureID = LoadCubemap(faces);

    skyboxShader = new Shader("../shaders/skybox.vs", "../shaders/skybox.fs");

    // Crear VAO/VBO del cubo
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // ---------- SHADERS ----------
    shader = new Shader("../shaders/basic.vs", "../shaders/basic.fs");
    grassShader = new Shader("../shaders/terrain.vs", "../shaders/terrain.fs");
    outliningShader = new Shader("../shaders/outlining.vs", "../shaders/outlining.fs");


    if (shader->ID == 0) {
        std::cerr << "Error compilando basic shader\n";
    }
    if (grassShader->ID == 0) {
        std::cerr << "Error compilando terrain shader\n";
        std::exit(-1);
    }

    // ---------- LUZ ----------
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, 10.0f, 5.0f);

    grassShader->Activate();
    glUniform4f(glGetUniformLocation(grassShader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(grassShader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // ---------- TERRENO ----------
    terrain = new Terrain("../textures/isla2.png", 100.0f, 1.0f); // Altura un poco más alta para verlo mejor

    // ---------- TEXTURAS ----------
    grassTextureID = LoadTexture("../textures/grass.png");
    rockTextureID = LoadTexture("../textures/rock.png");
    sandTextureID = LoadTexture("../textures/sand.png");

    if (grassTextureID == 0) std::cerr << "Warning: grass.png no cargó (ID=0)\n";
    if (rockTextureID == 0) std::cerr << "Warning: rock.png no cargó (ID=0)\n";

    crowModel = new Model("../models/crow/scene.gltf");
	crowOutline = new Model("../models/crow-outline/scene.gltf");

	crowModel1 = new Model("../models/crow/scene.gltf");
	crowOutline1 = new Model("../models/crow-outline/scene.gltf");

	crowModel2 = new Model("../models/crow/scene.gltf");
	crowOutline2 = new Model("../models/crow-outline/scene.gltf");

	crowModel3 = new Model("../models/crow/scene.gltf");
	crowOutline3 = new Model("../models/crow-outline/scene.gltf");

	crowModel4 = new Model("../models/crow/scene.gltf");
	crowOutline4 = new Model("../models/crow-outline/scene.gltf");

	crowModel5 = new Model("../models/crow/scene.gltf");
	crowOutline5 = new Model("../models/crow-outline/scene.gltf");

	crowModel6 = new Model("../models/crow/scene.gltf");
	crowOutline6 = new Model("../models/crow-outline/scene.gltf");

	crowModel7 = new Model("../models/crow/scene.gltf");
	crowOutline7 = new Model("../models/crow-outline/scene.gltf");

	crowModel8 = new Model("../models/crow/scene.gltf");
	crowOutline8 = new Model("../models/crow-outline/scene.gltf");



	// ---------- AGUA ----------

    waterShader = new Shader("../shaders/water.vs", "../shaders/water.fs");  // o water.vs si es separado
    water = new Water(0.1f, 2000.0f);  // Nivel del agua a Y=0.5, plano muy grande
}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 5000.0f);

        if (terrain) {
            grassShader->Activate();

            // Enviar la matriz combinada de cámara (view * proj)
            glUniformMatrix4fv(glGetUniformLocation(grassShader->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.cameraMatrix));

            // Posición de la cámara
            glUniform3fv(glGetUniformLocation(grassShader->ID, "camPos"), 1, glm::value_ptr(camera.Position));

            // Matriz modelo (identidad)
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(grassShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            // Bind texturas
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, grassTextureID);
            glUniform1i(glGetUniformLocation(grassShader->ID, "grassTex"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, rockTextureID);
            glUniform1i(glGetUniformLocation(grassShader->ID, "rockTex"), 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, sandTextureID);
            glUniform1i(glGetUniformLocation(grassShader->ID, "sandTex"), 2);

            // Dibujar terreno
            terrain->Draw(grassShader->ID);
        }

        if (crowModel)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(-50.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
             treePos.y = terrain->GetHeight(treePos.x, treePos.z)-1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

			glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
			glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel->Draw(*shader, camera, model);

			glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
			glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
			glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño
            
            if (crowOutline)
                crowOutline->Draw(*outliningShader, camera, model1);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel1)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(50.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel1->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño


            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel2)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(-150.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel2->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

          

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }

        if (crowModel3)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(150.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel3->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

          

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel4)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(200.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel4->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

         
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel5)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(250.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel5->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

          

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel6)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(400.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel6->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

      

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel7)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(-300.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel7->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (crowModel8)
        {
            shader->Activate();

            glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), 1, 1, 1, 1);
            glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), 5, 10, 5);
            glUniform3fv(glGetUniformLocation(shader->ID, "camPos"), 1,
                glm::value_ptr(camera.Position));

            // Posición del árbol en la isla
            glm::vec3 treePos = glm::vec3(-400.0f, 0.0f, 15.0f);

            // (opcional) ajustar altura al terreno
            treePos.y = terrain->GetHeight(treePos.x, treePos.z) - 1.0f;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, treePos);
            model = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilFunc(GL_ALWAYS, 1, 0xFF); // Establecer valor del stencil a 1 al dibujar el modelo
            glStencilMask(0xFF); // Habilitar escritura en el buffer de stencil

            crowModel8->Draw(*shader, camera, model);

            glStencilFunc(GL_NOTEQUAL, 1, 0X0FF); // Solo dibujar donde el stencil no es 1  
            glStencilMask(0x00); // Deshabilitar escritura en el buffer de stencil
            glDisable(GL_DEPTH_TEST); // Deshabilitar prueba de profundidad para el contorno
            outliningShader->Activate();
            glUniform1f(glGetUniformLocation(outliningShader->ID, "outlining"), 1.08f);

            glm::mat4 model1 = glm::mat4(1.0f);
            model1 = glm::translate(model, treePos);
            model1 = glm::scale(model, glm::vec3(1.0f)); // tamaño

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        if (water) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            waterShader->Activate();
            glUniformMatrix4fv(glGetUniformLocation(waterShader->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.cameraMatrix));
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(waterShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            water->Draw(waterShader->ID, (float)glfwGetTime());

            glDisable(GL_BLEND);
        }

        // Dibujar skybox al final
     // Dibujar skybox al final
        glDepthFunc(GL_LEQUAL);  // permite dibujar cuando depth == 1.0

        skyboxShader->Activate();

        // 1. Matriz de proyección (igual que usas en camera.updateMatrix)
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height,
            0.1f,
            5000.0f
        );

        // 2. Matriz view SIN traslación: cámara en (0,0,0) mirando en la misma dirección
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 0.0f),      // cámara en origen
            camera.Orientation,               // dirección hacia donde mira (debe ser normalizado)
            camera.Up                         // vector up de la cámara
        );

        // Enviar uniforms
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
        glUniform1i(glGetUniformLocation(skyboxShader->ID, "skybox"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);  // restaurar
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    if (shader) { shader->Delete(); delete shader; shader = nullptr; }
    if (grassShader) { grassShader->Delete(); delete grassShader; grassShader = nullptr; }
    if (terrain) { delete terrain; terrain = nullptr; }

    if (grassTextureID) glDeleteTextures(1, &grassTextureID);
    if (rockTextureID) glDeleteTextures(1, &rockTextureID);
    if (sandTextureID) glDeleteTextures(1, &sandTextureID);

    if (waterShader) { waterShader->Delete(); delete waterShader; }
    if (water) { delete water; }
    if (crowModel)
    {
        delete crowModel;
        crowModel = nullptr;
    }
    if( crowOutline)
    {
        delete crowOutline;
        crowOutline = nullptr;
	}
    if (crowModel1)
    {
        delete crowModel1;
        crowModel1 = nullptr;
	}
    if (crowOutline1)
    {
        delete crowOutline1;
        crowOutline1 = nullptr;
	}
    if (crowModel2)
    {
        delete crowModel2;
		crowModel2 = nullptr;

	}
    if (crowOutline2)
    {
        delete crowOutline2;
		crowOutline2 = nullptr;
	}
    if (crowModel3)
    {
        delete crowModel3;
    }
    if (crowOutline3)
    {
        delete crowOutline3;
	}
    if (crowModel4)
    {
        delete crowModel4;
	}
    if (crowOutline4)
    {
        delete crowOutline4;
    }
    if (crowModel5)
    {
        delete crowModel5;
    }
    if (crowOutline5)
    {
        delete crowOutline5;
	}
    if (crowModel6)
    {
        delete crowModel6;
    }
    if (crowOutline6)
    {
        delete crowOutline6;
    }
    if (crowModel7)
    {
        delete crowModel7;
	}
    if (crowOutline7)
    {
        delete crowOutline7;
    }

    if (crowModel8)
    {
        delete crowModel8;
	}
    if (crowOutline8)
    {
        delete crowOutline8;
	}

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLuint App::LoadTexture(const std::string& path) {
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);

    if (data) {
        GLenum internalFormat = GL_RGB;
        GLenum dataFormat = GL_RGB;

        if (channels == 1) { internalFormat = GL_RED;  dataFormat = GL_RED; }
        else if (channels == 3) { internalFormat = GL_RGB; dataFormat = GL_RGB; }
        else if (channels == 4) { internalFormat = GL_RGBA; dataFormat = GL_RGBA; }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Textura cargada: " << path << " (" << w << "x" << h << ", " << channels << " canales)\n";
        stbi_image_free(data);
    }
    else {
        std::cerr << "ERROR: No se pudo cargar textura: " << path << std::endl;
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    return textureID;
}