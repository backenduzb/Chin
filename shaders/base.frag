#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D diffuseTexture;
uniform bool hasTexture;
uniform vec3 color;

void main()
{
    if (hasTexture)
    {
        FragColor = texture(diffuseTexture, vTexCoord);
    }
    else
    {
        FragColor = vec4(color, 1.0);
    }
}