#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Water {
public:
    Water(float level = 0.0f, float size = 1000.0f);  // Nivel Y del agua y tamaño del plano
    ~Water();

    void Draw(GLuint shaderProgram, float time);  // Pasamos tiempo para animación

private:
    void setupBuffers();

    unsigned int VAO, VBO, EBO;
    float waterLevel;
    float planeSize;
};