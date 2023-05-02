#version 430
in vec2 frag_UV;
in vec3 frag_Normal;
in vec3 frag_Position;
in vec3 frag_Light;
in vec3 frag_Tangent;
in vec3 frag_Bitangent;

out vec4 frag_color;

uniform vec3 Light;
uniform mat4 LightModelView;
uniform sampler2D Texture;
uniform sampler2D Bump;
uniform sampler2D Specular;
uniform sampler2D Cloud;
uniform sampler2D Night;
uniform sampler2D Explosion;
uniform double Time;

void main() {
	vec3 frag_Light= vec3(LightModelView * vec4(Light, 1.0));

	vec3 rgb = vec3(texture2D(Bump, frag_UV));
	rgb.r = float((rgb.r - 0.5) * 2);
	rgb.g = float((rgb.g - 0.5) * 2);
	rgb.b = float((rgb.b - 0.5) * 2);
	vec3 specmap = vec3(texture2D(Specular, frag_UV));

	vec3 i = normalize(frag_Light - frag_Position);
	//vec3 n = normalize(frag_Normal);
	vec3 n = normalize(rgb.r * frag_Tangent + rgb.g * frag_Bitangent + rgb.b * frag_Normal);

	vec3 v = normalize(-frag_Position);
	//vec3 r = reflect(-i,n);
	vec3 h = normalize(i + v);

	vec3 ambient = 0.5*vec3(0.3f, 0.3f, 0.3f);
	vec3 diffuse = 1.0*vec3(0.7f, 0.7f, 0.7f)*max(dot(n,i),0.0);
	//vec3 specular = 0.5*vec3(0.5f, 0.5f, 0.5f)*pow(max(dot(r, v), 0.0), 3.6);	
	//vec3 specular = 1.0*vec3(0.5f, 0.5f, 0.5f)*pow(max(dot(n, h), 0.0), 7.2);
	vec3 specular = 0.8*specmap*pow(max(dot(n, h), 0.0), 7.2);

	vec4 earth = texture2D(Texture, frag_UV);
	vec4 clouds =  texture2D(Cloud, frag_UV);
	vec4 night = texture2D(Night, frag_UV);


	vec3 map1 = earth.rgb * (ambient+diffuse) + (specular*clouds.g) * (1.0-clouds.r) + clouds.r * diffuse;
	vec3 map2 = night.rgb * (1.0-clouds.r) * 2.0;

	//frag_color = vec4(ambient+diffuse,1)*earth+vec4(specular,1);
	vec3 color = map1;
	if(max(dot(n,i),0.0)<0.1)
		color=mix(map1,map2,(max(dot(n,i),0.0)+0.1) * 5.0);

	float alpha;

	if(10 - 0.003 * Time<=1.25)
	{
		alpha = 0.0001 * float(Time);
		alpha = min(alpha,1);

		color=color*(1-alpha)+texture2D(Explosion, frag_UV).rgb*alpha;
	}

	frag_color=vec4(color,1);
}
