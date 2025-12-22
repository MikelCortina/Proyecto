#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 camMatrix;
uniform mat4 model;
uniform float time;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords + time * 0.05;  // Movimiento lento de textura
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = camMatrix * worldPos;
}