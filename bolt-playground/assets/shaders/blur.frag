#version 330 core

uniform sampler2D	uTex0;

in vec4		Color;
//in vec3		Normal;
in vec2		TexCoord0;
//in vec2 	WindowSize;

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

uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 1.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 2.0162162162 );


void main( void )
{
	//vec2 uv = vec2(gl_FragCoord.xy/ WindowSize.xy);

	vec3 color = vec3( 0.0, 1.0, 1.0 );
	//vec3 color = texture( uTex0, TexCoord0 ).rgg;
	//vec4 blured = blur13( uTex0, TexCoord0, vec2(1024,768), vec2( 0.5, 1.5 ) );
	//vec3 color = blured.rgb ;
	//vec4 cc =0.1* texture( uTex0, TexCoord0 );
	//for (int i=1; i<5; i++) {
	//	//cc += texture( uTex0, ( vec2(TexCoord0)+vec2(offset[i], offset[i]) )) * weight[i];
	//	cc += texture( uTex0, ( vec2(TexCoord0)- vec2(0.0, 0.0) ) ) * 20.0;
	//}
	//cc = vec4(0.0);
	//cc.r = gl_FragCoord.x / 1024.0;

	oColor = vec4( color.rgb, 1.0 );
}