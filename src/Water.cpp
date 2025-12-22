#include "Water.h"
#include <iostream>

Water::Water(float level, float size)
    : waterLevel(level), planeSize(size)
{
    setupBuffers();
}

Water::~Water() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Water::setupBuffers() {
    // Plano grande centrado en origen
    float half = planeSize / 2.0f;

    std::vector<glm::vec3> vertices = {
        glm::vec3(-half, waterLevel, -half),
        glm::vec3(half, waterLevel, -half),
        glm::vec3(half, waterLevel,  half),
        glm::vec3(-half, waterLevel,  half)
    };

    std::vector<glm::vec2> texCoords = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(10.0f, 0.0f),  // Repetimos textura
        glm::vec2(10.0f, 10.0f),
        glm::vec2(0.0f, 10.0f)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoords;
    };

    std::vector<Vertex> vertexData(4);
    for (int i = 0; i < 4; ++i) {
        vertexData[i].Position = vertices[i];
        vertexData[i].TexCoords = texCoords[i];
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // TexCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Water::Draw(GLuint shaderProgram, float time) {
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}