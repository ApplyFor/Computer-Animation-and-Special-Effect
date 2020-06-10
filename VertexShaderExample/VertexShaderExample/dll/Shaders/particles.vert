#version 430
layout(location = 0) in vec3 pos;

layout(location = 1) in vec3 speed;

layout(location = 2) in float fade;

layout(location = 3) in float life;

uniform mat4 Projection;
uniform mat4 ModelView;
out float frag_life;

void main() {
	gl_Position = Projection * ModelView * vec4(pos, 1.0);
	frag_life = life;
}