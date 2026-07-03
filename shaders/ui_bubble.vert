#version 330 core
layout (location = 0) in vec4 vertex; 

out vec2 TexCoords;

uniform mat4 u_Projection;
uniform vec2 u_Position; 
uniform vec2 u_QuadSize;
uniform float u_Scale;

void main() {
    TexCoords = vertex.zw;
    
    vec2 center = u_Position + u_QuadSize * 0.5;
    vec2 scaledSize = u_QuadSize * u_Scale;
    vec2 pos = center + (vertex.xy - 0.5) * scaledSize;
    
    gl_Position = u_Projection * vec4(pos, 0.0, 1.0);
}
