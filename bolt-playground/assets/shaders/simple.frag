#version 150

uniform sampler2D uTex0;

in vec4		Color;
//in vec3		Normal;
in vec2		TexCoord;

out vec4 	oColor;

void main( void )
{
    const float epsilon = 0.01;
    vec2 uv = TexCoord;
   
    float c =  exp(-6.0 * abs(uv.y - 0.5));
    c += smoothstep(0.5 - epsilon, 0.5, uv.y) - smoothstep(0.5, 0.5 + epsilon, uv.y);
    c *= 1.0 -smoothstep(0.1, 0.0, 1.0-uv.x);
   
    
    oColor = c * Color;
    
}
