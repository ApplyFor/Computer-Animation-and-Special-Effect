#version 430
layout(location = 0) in vec3 position;

layout(location = 1) in vec2 texcoord;

layout(location = 2) in vec3 normal;

out vec2 geom_UV;
out vec3 geom_Normal;
out vec3 geom_Position;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 Light;
uniform sampler2D Texture;

void main() {
	geom_UV = texcoord;
	gl_Position = Projection * ModelView * vec4(position, 1.0);
	mat3 NormalMatrix = mat3(transpose(inverse(ModelView)));
	geom_Normal = NormalMatrix * normal;
	geom_Position= vec3(ModelView * vec4(position, 1.0));
	//geom_Light= vec3(LightModelView * vec4(Light, 1.0));
}
