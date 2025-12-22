#include "Terrain.h"
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>

Terrain::Terrain(const std::string& heightmapPath, float heightScale, float sizeScale)
    : heightScale(heightScale), sizeScale(sizeScale)
{
    loadHeightmap(heightmapPath);

    // Si falló la carga o es demasiado pequeño, creamos un terreno plano mínimo para evitar crash
    if (width < 2 || height < 2) {
        std::cerr << "Heightmap inválido o no cargado. Creando terreno plano de emergencia.\n";
        width = height = 2;

        vertices = {
            glm::vec3(-10.0f, 0.0f, -10.0f),
            glm::vec3(10.0f, 0.0f, -10.0f),
            glm::vec3(-10.0f, 0.0f,  10.0f),
            glm::vec3(10.0f, 0.0f,  10.0f)
        };

        texCoords = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f)
        };

        normals.assign(4, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    generateMesh();
    calculateNormals();
    setupBuffers();
}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Terrain::loadHeightmap(const std::string& path) {
    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 1); // Forzamos canal gris

    if (!data) {
        std::cerr << "Error cargando heightmap: " << path << std::endl;
        return;
    }

    vertices.resize(width * height);
    texCoords.resize(width * height);

    // Centramos el terreno en el origen
    float offsetX = -(width * sizeScale) / 2.0f;
    float offsetZ = -(height * sizeScale) / 2.0f;

    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float y = (data[z * width + x] / 255.0f) * heightScale;

            vertices[z * width + x] = glm::vec3(
                x * sizeScale + offsetX,
                y,
                z * sizeScale + offsetZ
            );

            texCoords[z * width + x] = glm::vec2(
                static_cast<float>(x) / (width - 1),
                static_cast<float>(z) / (height - 1)
            );
        }
    }

    stbi_image_free(data);
}

void Terrain::generateMesh() {
    indices.clear();

    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = topLeft + width;
            unsigned int bottomRight = bottomLeft + 1;

            // Triángulo 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triángulo 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void Terrain::calculateNormals() {
    normals.assign(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v0 = vertices[i0];
        glm::vec3 v1 = vertices[i1];
        glm::vec3 v2 = vertices[i2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }

    for (auto& n : normals) {
        if (glm::length(n) > 0.0001f) {
            n = glm::normalize(n);
        }
        else {
            n = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

void Terrain::setupBuffers() {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    std::vector<Vertex> vertexData(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertexData[i].Position = vertices[i];
        vertexData[i].Normal = normals[i];
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

    // layout(location = 0) Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // layout(location = 1) Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // layout(location = 2) TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Terrain::Draw(GLuint shaderProgram) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}