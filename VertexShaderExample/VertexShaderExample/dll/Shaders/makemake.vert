#version 430
layout(location = 0) in vec3 position;

layout(location = 1) in vec2 texcoord;

layout(location = 2) in vec3 normal;

out vec2 frag_UV;
out vec3 frag_Normal;
out vec3 frag_Position;
out vec3 frag_Light;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 Light;
uniform mat4 LightModelView;

void main() {
	frag_UV = texcoord;
	gl_Position = Projection * ModelView * vec4(position, 1.0);
	mat3 NormalMatrix = mat3(transpose(inverse(ModelView)));
	frag_Normal = NormalMatrix * normal;
	frag_Position= vec3(ModelView * vec4(position, 1.0));
	frag_Light= vec3(LightModelView * vec4(Light, 1.0));
}
