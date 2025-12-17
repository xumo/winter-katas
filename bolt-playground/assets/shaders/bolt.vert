#version 410

uniform mat4    uModelViewProjection;
uniform float uTime;


layout (location = 0) in vec2         iStartPosition;
layout (location = 1) in vec2         iEndPosition;
layout (location = 2) in int          iDivisions;

layout (location = 0) out vec2    start;
layout (location = 1) out vec2    end;
layout (location = 2) out int     divisions;

void main( void )
{
    gl_Position    = uModelViewProjection * vec4(iStartPosition.x, iStartPosition.y, 0.0, 1.0);

    vec4 ss     = uModelViewProjection * vec4(iStartPosition.x, iStartPosition.y, 0.0, 1.0);
    start       = ss.xy;
    vec4 ee     = uModelViewProjection * vec4(iEndPosition.x, iEndPosition.y, 0.0, 1.0);
    end         = ee.xy;
    divisions   = iDivisions;
}
