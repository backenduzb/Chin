#version 330 core

out vec4 FragColor;

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vFragPos;

uniform sampler2D diffuseTexture;
uniform bool hasTexture;

void main() {
    vec3 lightPos = vec3(5.0, 5.0, 5.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 resultColor = ambient + diffuse;
    
    if (hasTexture) {
        vec4 texColor = texture(diffuseTexture, vTexCoord);
        if (texColor.a < 0.1) discard;
        FragColor = vec4(resultColor * texColor.rgb, texColor.a);
    } else {
        FragColor = vec4(resultColor * vec3(0.8, 0.8, 0.8), 1.0);
    }
}
