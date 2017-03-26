#ifdef GL_ES
precision mediump float;
#else
#define lowp 
#define mediump 
#define highp 
#endif

attribute highp vec3 Position;

uniform highp mat4 modelProjectionMatrix;

void main(){
	gl_Position = modelProjectionMatrix * vec4(Position, 1.0);
}