// frag
#version 410

in vec3 gColor;
in vec2 gTexCoord;

out vec4 outColor;

void main()
{
    
    const float epsilon = 0.01;
    vec2 uv = gTexCoord;
   
    float c =  exp(-6.0 * abs(uv.y - 0.5));
    c += smoothstep(0.5 - epsilon, 0.5, uv.y) - smoothstep(0.5, 0.5 + epsilon, uv.y);
    c *= 1.0 -smoothstep(0.05, 0.0, 1.0-uv.x);
    c *= 1.0 -smoothstep(0.05, 0.0, uv.x);
    
    // outColor = vec4(c * gColor, c);
    outColor = vec4(1.0) - vec4(gColor, c);
}
