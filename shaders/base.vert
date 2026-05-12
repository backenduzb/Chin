#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aJoints;
layout(location = 4) in vec4 aWeights;

out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 jointMatrices[256];
uniform bool isSkinned;

void main() {
    mat4 skinMat = mat4(0.0);
    if (isSkinned) {
        skinMat += aWeights.x * jointMatrices[int(aJoints.x)];
        skinMat += aWeights.y * jointMatrices[int(aJoints.y)];
        skinMat += aWeights.z * jointMatrices[int(aJoints.z)];
        skinMat += aWeights.w * jointMatrices[int(aJoints.w)];
        
        float totalWeight = aWeights.x + aWeights.y + aWeights.z + aWeights.w;
        if (totalWeight > 0.0) {
            skinMat /= totalWeight;
        } else {
            skinMat = mat4(1.0);
        }
    } else {
        skinMat = mat4(1.0);
    }

    vFragPos = vec3(model * skinMat * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(model * skinMat))) * aNormal;
    vTexCoord = aTexCoord;
    gl_Position = projection * view * vec4(vFragPos, 1.0);
}
