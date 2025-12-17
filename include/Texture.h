#pragma once
#include <string>
#include <glad/glad.h>
#include <iostream>

class Texture {
public:
    Texture(const std::string& filepath);
    ~Texture();

    void bind(unsigned int unit = 0) const;
    void unbind() const;

private:
    GLuint textureID;
    int width, height, channels;
    unsigned char* data;
};
