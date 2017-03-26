#ifdef GL_ES
precision mediump float;
#else
#define lowp 
#define mediump 
#define highp 
#endif

varying mediump vec2 TexCoord0;

attribute highp vec3 Position;
attribute highp vec2 TexCoord;

uniform highp mat4 modelProjectionMatrix;

void main(){
	gl_Position = modelProjectionMatrix * vec4(Position, 1.0);
	TexCoord0 = TexCoord; 
}