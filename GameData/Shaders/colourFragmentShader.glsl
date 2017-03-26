#ifdef GL_ES
precision mediump float;
#else
#define lowp 
#define mediump 
#define highp 
#endif

uniform vec4 inputColour;

void main(){
	gl_FragColor = vec4(inputColour);
} 