#ifdef GL_ES
precision mediump float;
#else
#define lowp 
#define mediump 
#define highp 
#endif

varying mediump vec2 TexCoord0;

uniform sampler2D gSampler;
uniform vec4 amtran;

void main(){
	gl_FragColor = texture2D(gSampler, TexCoord0.xy) * amtran;

} 
