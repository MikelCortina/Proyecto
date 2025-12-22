#version 330 core

uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D sandTex;

uniform vec4 lightColor;
uniform vec3 lightPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);

    // Pendiente: 0.0 = plano, 1.0 = vertical
    float slope = 1.0 - norm.y;

    // Sample texturas (ajusta los multiplicadores para que se repitan bien)
    vec4 sand  = texture(sandTex,  TexCoords * 12.0);
    vec4 grass = texture(grassTex, TexCoords * 8.0);
    vec4 rock  = texture(rockTex,  TexCoords * 6.0);

    // 1. ROCA: aparece a partir de pendientes más suaves (~30°-35° en vez de 55°)
    float rockFactor = smoothstep(0.35, 0.65, slope);  
    // Antes era 0.55-0.8 → ahora más temprano y transición más suave

    // 2. ARENA: solo en zonas bajas Y muy planas
    float heightLow = smoothstep(0.0, 2.5, FragPos.y);           // Ajusta 2.5 según tu terreno
    float flatness = 1.0 - smoothstep(0.0, 0.25, slope);        // Muy plano
    float sandFactor = (1.0 - heightLow) * flatness;
    sandFactor = clamp(sandFactor, 0.0, 1.0);

    // Mezcla: primero grass + rock
    vec4 grassRock = mix(grass, rock, rockFactor);

    // Luego añadir arena solo donde toca
    vec4 baseColor = mix(grassRock, sand, sandFactor);

    // Iluminación
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * lightColor;
    vec4 ambient = 0.3 * lightColor;

    FragColor = (ambient + diffuse) * baseColor;
}