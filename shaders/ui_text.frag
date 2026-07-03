#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D u_TextTexture;
uniform vec3 u_TextColor;
uniform float u_Alpha;

void main() {
    float sampled = texture(u_TextTexture, TexCoords).r;
    FragColor = vec4(u_TextColor, sampled * u_Alpha);
}
