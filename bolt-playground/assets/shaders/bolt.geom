#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 64) out;

in vec2    start[];
in vec2    end[];
in int   divisions[];

out vec3 gColor;
out vec2 gTexCoord;

uniform int uNumSides = 4;
uniform float uOffset = 4.0;


uniform float uTime;

const float PI = 3.1415926;




float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void segment(vec2 s, vec2 e) {
    
    vec2 direction = normalize( e - s );
    vec2 normal = ( vec2(-direction.y, direction.x) );
    float halfWidth = 0.050f;
    vec2 offset = normal * halfWidth;
    
    gl_Position = vec4(vec2(s - offset), 0.0f, 1.0f);
    gColor = vec3(1.0, 0.0, 0.0);
    gTexCoord = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = vec4(vec2(s + offset), 0.0f, 1.0f);
    gColor = vec3(1.0, 0.0, 0.0);
    gTexCoord = vec2(0.0, 0.0);
    EmitVertex();
    /*{
        gl_Position = vec4(vec2(e + offset), 0.0f, 1.0f);
        gColor = vec3(0.0, 0.0, 1.0);
        gTexCoord = vec2(1.0, 0.0);
        EmitVertex();
    }*/
    gl_Position = vec4(vec2(e - offset), 0.0f, 1.0f);
    gColor = vec3(0.0, 0.0, 1.0);
    gTexCoord = vec2(1.0, 1.0);
    
    EmitVertex();
    
    gl_Position = vec4(vec2(e + offset), 0.0f, 1.0f);
    gColor = vec3(0.0, 0.0, 1.0);
    gTexCoord = vec2(1.0, 0.0);
    EmitVertex();
   /* {
        gl_Position = vec4(vec2(s - offset), 0.0f, 1.0f);
        gColor = vec3(1.0, 0.0, 0.0);
        gTexCoord = vec2(0.0, 1.0);
        EmitVertex();
    }*/
}


void main()
{
    gColor = vec3(1.0, 1.0, 1.0);
    vec2 s = start[0];
    vec2 e = end[0];
    int divs = divisions[0];
   
    vec2 aV = s;
    vec2 bV, eV, d, n;
    float m, nm, ex;
    
    float time = 0.000001* uTime;
    float r = rand(time * s);
    
    /*
     for (int i=0; i<numDivs; i++) {
             nX = (bV.x + aV.x)*0.5;
             nX += (Rand::randFloat() - 0.5)*aumento;
             nY = (bV.y + aV.y)*0.5;
             nY += (Rand::randFloat() - 0.5)*aumento;
             
             aV = Vec2f(nX, nY);
             puntos.push_back( aV );
         }
         
     */
    
    float nX, nY;
    for (int i = 0; i < divs; i++)
    {
        //m = 0.1 + 0.4f * r;
        //nm = 0.5f - r;
        //ex = rand(e );
        //d = e - aV;
        //n = 0.4f * nm * vec2(-d.y, d.x);
        //bV = aV + m * d + n;
        //eV = aV + (bV - aV) * (1.0 + ex);
        nX = (e.x + aV.x)*0.5;
        nY = (e.y + aV.y)*0.5;
        nX += (rand(vec2(time, float(i))) - 0.5) * uOffset;
        nY += (rand(vec2(float(i), time)) - 0.5) * uOffset;
        bV = vec2(nX, nY);
        
        segment(aV, bV);
        aV = bV;
    }
  
       
    segment(aV, e);
  
    EndPrimitive();
}
