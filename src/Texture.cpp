#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>  // Para std::cerr

Texture::Texture(const std::string& filepath)
    : textureID(0), data(nullptr), width(0), height(0), channels(0)
{
    stbi_set_flip_vertically_on_load(true); // Voltear la imagen al cargarla

    data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Error cargando textura: " << filepath << "\n";
        std::cerr << "Motivo STB: " << stbi_failure_reason() << "\n";
        return;
    }

    // Determinar formatos según el número de canales
    GLenum internalFormat;
    GLenum dataFormat;

    if (channels == 1) {
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    }
    else if (channels == 3) {
        internalFormat = GL_RGB;
        dataFormat = GL_RGB;
    }
    else if (channels == 4) {
        internalFormat = GL_RGBA;
        dataFormat = GL_RGBA;
    }
    else {
        std::cerr << "Número de canales no soportado (" << channels << "): " << filepath << "\n";
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Parámetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Subir los datos a la GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    // Generar mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Bonus: Para imágenes grayscale, hacer que se vea gris en lugar de rojo
    if (channels == 1) {
        GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    // Liberar datos de CPU
    stbi_image_free(data);
    data = nullptr;

    // Desbindear
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}