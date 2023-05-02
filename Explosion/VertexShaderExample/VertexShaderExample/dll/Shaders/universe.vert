#version 430
layout(location = 0) in vec3 position;

layout(location = 1) in vec2 texcoord;

layout(location = 2) in vec3 normal;

out vec2 frag_UV;

uniform mat4 Projection;
uniform mat4 ModelView;

void main() {
	frag_UV = texcoord;
	gl_Position = Projection * ModelView * vec4(position, 1.0);
}
