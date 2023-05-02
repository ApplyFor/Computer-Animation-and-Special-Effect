#version 430
in vec2 frag_UV;
in vec3 frag_Normal;
in vec3 frag_Position;
in vec3 frag_Light;

out vec4 frag_color;

uniform sampler2D MakeMake;

void main() {
	vec3 i = normalize(frag_Light - frag_Position);
	vec3 n = normalize(frag_Normal);
	vec3 v = normalize(-frag_Position);
	//vec3 r = reflect(-i,n);
	vec3 h = normalize(i + v);

	vec3 ambient = 1.0*vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = 1.0*vec3(0.5f, 0.5f, 0.5f)*max(dot(n,i),0.0);
	//vec3 specular = 0.5*vec3(0.8f, 0.8f, 0.8f)*pow(max(dot(r, v), 0.0), 3.6);
	vec3 specular = 0.5*vec3(0.8f, 0.8f, 0.8f)*pow(max(dot(n, h), 0.0), 3.6);

	frag_color = vec4(ambient+diffuse,1)*texture(MakeMake, frag_UV)+vec4(specular,1);
}
