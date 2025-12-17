#include "App.h"
#include "Texture.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h" 


static bool checkShaderCompileStatus(unsigned int shader, const std::string& name) {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen + 1);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << "Error compilando shader (" << name << "):\n" << log.data() << "\n";
        return false;
    }
    return true;
}

static bool checkProgramLinkStatus(unsigned int program) {
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen + 1);
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        std::cerr << "Error enlazando programa:\n" << log.data() << "\n";
        return false;
    }
    return true;
}

App::App() {
    init();
}

App::~App() {
    cleanup();
}

std::string App::loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo de shader: " << filepath << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void App::CreateVBO() {
    GLfloat vertices[] = {
        // pos              // color        // uv
        -0.5f,0.0f,-0.5f,   1,0,0,           0,0,
         0.5f,0.0f,-0.5f,   0,1,0,           1,0,
         0.5f,0.0f, 0.5f,   0,0,1,           1,1,
        -0.5f,0.0f, 0.5f,   1,1,0,           0,1,
         0.0f,0.8f, 0.0f,   1,1,1,           0.5,0.5
    };

    GLuint indices[] = {
        // Base
        0, 1, 2,
        2, 3, 0,

        // Caras laterales
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void App::init() {
    vertexShaderSource = loadShaderSource("../shaders/basic.vs");
    fragmentShaderSource = loadShaderSource("../shaders/basic.fs");

    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        exit(-1);
    }

    window = glfwCreateWindow(width, height, "Ejemplo OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        exit(-1);
    }

    glViewport(0, 0, width, height);

    // Compilar shaders
    const char* vsrc = vertexShaderSource.c_str();
    const char* fsrc = fragmentShaderSource.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsrc, nullptr);
    glCompileShader(vertexShader);
    if (!checkShaderCompileStatus(vertexShader, "vertex")) exit(-1);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsrc, nullptr);
    glCompileShader(fragmentShader);
    if (!checkShaderCompileStatus(fragmentShader, "fragment")) exit(-1);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    if (!checkProgramLinkStatus(shaderProgram)) exit(-1);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

	glEnable(GL_DEPTH_TEST);

    CreateVBO();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Crear textura usando la nueva clase
    textureObj = new Texture("../textures/Textura1.png");
}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        prevTime = glfwGetTime();
		double crntTime = glfwGetTime();

        if(crntTime-prevTime >=1/60.0)
        {
            rotation += 0.5f;
			prevTime = crntTime;
		}

        //Inicializa las matrices para que no sean nulas

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -3.0f));
        proj = glm::perspective(glm::radians(45.0f),(float)width / (float)height,0.1f,100.0f);

		int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

                
        textureObj->bind();

        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }

    delete textureObj;
    textureObj = nullptr;

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
