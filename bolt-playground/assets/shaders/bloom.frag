#version 330 core

uniform sampler2D	uTex0;

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord0;
in vec2 	WindowSize;

out vec4 	oColor;

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += texture(image, uv) * 0.1964825501511404;
	color += texture(image, uv + (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv - (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv + (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv - (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv + (off3 / resolution)) * 0.010381362401148057;
	color += texture(image, uv - (off3 / resolution)) * 0.010381362401148057;
	return color;
}


void main( void )
{
	vec2 uv = vec2(TexCoord0.xy / WindowSize.xy);
	//vec3 color = vec3( 1.0, 1.0, 1.0 )* diffuse;
	//vec3 color = texture( uTex0, TexCoord0 ).rgg;
	vec4 blured = blur13( uTex0, TexCoord0, WindowSize.xy, vec2( 1.0, 1.0 ) );
	vec3 color = blured.rgb;
	oColor = vec4( color, 1.0 );
}