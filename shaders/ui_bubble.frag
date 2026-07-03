#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform vec2 u_QuadSize;
uniform vec2 u_BubbleSize;
uniform float u_Alpha;
uniform int u_Direction; 

float sdRoundBox(vec2 p, vec2 b, vec4 r) {
    vec2 q = abs(p) - b;
    
    float radius = r.x; 
    if (p.x < 0.0 && p.y > 0.0) radius = r.z;      
    else if (p.x > 0.0 && p.y < 0.0) radius = r.y; 
    else if (p.x < 0.0 && p.y < 0.0) radius = r.w; 
    
    q += radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

void main() {
    vec2 center = u_QuadSize * 0.5;
    vec2 p = TexCoords * u_QuadSize - center;
    
    float fullRadius = min(u_BubbleSize.x, u_BubbleSize.y) * 0.35; 
    vec4 radii = vec4(fullRadius); 
    if (u_Direction == 3) {
        radii.x = 0.0; 
    } else if (u_Direction == 0) {
        radii.w = 0.0; 
    } else if (u_Direction == 1) {
        radii.y = 0.0; 
    } else if (u_Direction == 2) {
        radii.z = 0.0; 
    }
    
    float d = sdRoundBox(p, u_BubbleSize * 0.5, radii);
    
    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);
    
    float shadowBlur = 15.0;
    float shadowAlpha = 1.0 - smoothstep(0.0, shadowBlur, d);
    vec4 shadowColor = vec4(0.0, 0.0, 0.0, 0.2 * shadowAlpha);
    
    vec4 bgColor = vec4(1.0, 1.0, 1.0, 0.44);
    
    vec4 finalColor = mix(shadowColor, bgColor, alpha);
    
    FragColor = finalColor * u_Alpha;
}