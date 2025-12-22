#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform float time;

void main() {
    vec2 uv = TexCoords;
    
    // Ondas simples con sin/cos
    float wave = sin(uv.x * 10.0 + time * 2.0) * 0.01 +
                 sin(uv.y * 8.0 + time * 1.5) * 0.01;
    
    vec3 deepColor = vec3(0.0, 0.1, 0.3);
    vec3 shallowColor = vec3(0.0, 0.4, 0.6);
    vec3 waterColor = mix(deepColor, shallowColor, wave + 0.5);
    
    FragColor = vec4(waterColor, 0.7);  // Semi-transparente
}