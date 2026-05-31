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
#ifdef GL_ES
	// GLES2 disallows GL_REPEAT on NPOT textures.  The original desktop path
	// relied on repeat-mode UVs, so emulate repeat in shader space when the
	// sampler itself has to remain CLAMP_TO_EDGE.
	vec2 repeatedTexCoord = fract(TexCoord0.xy);
	gl_FragColor = texture2D(gSampler, repeatedTexCoord) * amtran;
#else
	gl_FragColor = texture2D(gSampler, TexCoord0.xy) * amtran;
#endif

}
