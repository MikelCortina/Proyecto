
#include "App.h"
#include "Texture.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Usa tus clases de buffers
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

// -------------------------------------------------------------
// Helpers para compilación de shaders
// -------------------------------------------------------------
static bool checkShaderCompileStatus(GLuint shader, const std::string& name) {
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

static bool checkProgramLinkStatus(GLuint program) {
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

// -------------------------------------------------------------
// App
// -------------------------------------------------------------
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

void App::CreateGeometry() {
    // Datos de vértices (pos[3], color[3], uv[2]) -> stride = 8 floats
    static GLfloat vertices[] = {
        // pos              // color        // uv
        -0.5f, 0.0f, -0.5f,  1, 0, 0,        0, 0,
         0.5f, 0.0f, -0.5f,  0, 1, 0,        1, 0,
         0.5f, 0.0f,  0.5f,  0, 0, 1,        1, 1,
        -0.5f, 0.0f,  0.5f,  1, 1, 0,        0, 1,
         0.0f, 0.8f,  0.0f,  1, 1, 1,        0.5f, 0.5f
    };

    // Índices (usa GLuint)
    static GLuint indices[] = {
        // Base
        0, 1, 2,
        2, 3, 0,
        // Caras laterales
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    // Crear VAO/VBO/EBO usando tus clases
    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices, sizeof(vertices));
    ebo = new EBO(indices, sizeof(indices)); // EBO se vincula al VAO activo

    // Atributos
    vao->LinkVBO(*vbo, /*layout*/0, /*numComponents*/3, GL_FLOAT, 8 * sizeof(float), (void*)0);                    // posición
    vao->LinkVBO(*vbo, /*layout*/1, /*numComponents*/3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));  // color
    vao->LinkVBO(*vbo, /*layout*/2, /*numComponents*/2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));  // uv

    vao->Unbind();
}

void App::init() {
    // Cargar shaders desde archivo
    vertexShaderSource = loadShaderSource("../shaders/basic.vs");
    fragmentShaderSource = loadShaderSource("../shaders/basic.fs");

    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        std::exit(-1);
    }

    window = glfwCreateWindow(width, height, "Ejemplo OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    glViewport(0, 0, width, height);

    // Compilar shaders manualmente (si no usas shaderClass.h)
    const char* vsrc = vertexShaderSource.c_str();
    const char* fsrc = fragmentShaderSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsrc, nullptr);
    glCompileShader(vertexShader);
    if (!checkShaderCompileStatus(vertexShader, "vertex")) std::exit(-1);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsrc, nullptr);
    glCompileShader(fragmentShader);
    if (!checkShaderCompileStatus(fragmentShader, "fragment")) std::exit(-1);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    if (!checkProgramLinkStatus(shaderProgram)) std::exit(-1);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    // Uniform sampler para textura
    GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(texLoc, 0); // GL_TEXTURE0

    // Profundidad y clear color
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Geometría con clases VAO/VBO/EBO
    CreateGeometry();

    // Cargar textura (tu clase Texture debe manejar el bind/unbind y el stb_image)
    textureObj = new Texture("../textures/Textura1.png");

    // Inicializar temporizador para animación
    prevTime = glfwGetTime();
}

void App::run() {
    mainLoop();
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Animación con delta time
        double crntTime = glfwGetTime();
        double delta = crntTime - prevTime;
        if (delta >= (1.0 / 60.0)) {
            rotation += 0.5f;          // ajusta velocidad si quieres
            prevTime = crntTime;
        }

        // Matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        // Usa 'rotation' o el tiempo, como prefieras
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -3.0f));
        proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // Dibujo
        vao->Bind();
        textureObj->bind();

        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        // Opcional: Unbind si quieres mantener estado limpio
        // textureObj->unbind(); // si tu clase lo proporciona
        vao->Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {
    // Borrar programa
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    // Borrar buffers usando sus métodos
    if (ebo) { ebo->Delete(); delete ebo; ebo = nullptr; }
    if (vbo) { vbo->Delete(); delete vbo; vbo = nullptr; }
    if (vao) { vao->Delete(); delete vao; vao = nullptr; }

    // Textura
    delete textureObj;
    textureObj = nullptr;

    // Ventana/GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
