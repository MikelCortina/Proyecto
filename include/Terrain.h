#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Terrain {
public:
    Terrain(const std::string& heightmapPath, float heightScale = 5.0f, float sizeScale = 1.0f);
    ~Terrain();

    void Draw(GLuint shaderProgram);

    float GetHeight(float worldX, float worldZ) const;

private:
    void loadHeightmap(const std::string& path);
    void generateMesh();
    void calculateNormals();
    void setupBuffers();

    // Buffers OpenGL
    unsigned int VAO, VBO, EBO;

    // Datos del terreno
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    int width = 0;
    int height = 0;

    float heightScale;
    float sizeScale;
};