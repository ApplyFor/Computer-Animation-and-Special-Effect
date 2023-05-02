#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec2 geom_UV[3];
in vec3 geom_Normal[3];
in vec3 geom_Position[3];

out vec2 frag_UV;
out vec3 frag_Normal;
out vec3 frag_Position;
out vec3 frag_Tangent;
out vec3 frag_Bitangent;

void main()
{
	vec3 tangent, bitangent;
	vec3 E1 = geom_Position[1] - geom_Position[0];
	vec3 E2 = geom_Position[2] - geom_Position[0];
	vec2 delUV1 = geom_UV[1] - geom_UV[0];
	vec2 delUV2 = geom_UV[2] - geom_UV[0];

	float f = 1.0f / (delUV1.x * delUV2.y - delUV2.x * delUV1.y);

	tangent.x = f * (delUV2.y * E1.x - delUV1.y * E2.x);
	tangent.y = f * (delUV2.y * E1.y - delUV1.y * E2.y);
	tangent.z = f * (delUV2.y * E1.z - delUV1.y * E2.z);
	//tangent = normalize(tangent);

	bitangent.x = f * (-1*delUV2.x * E1.x + delUV1.x * E2.x);
	bitangent.x = f * (-1*delUV2.x * E1.y + delUV1.x * E2.y);
	bitangent.x = f * (-1*delUV2.x * E1.z + delUV1.x * E2.z);
	//bitangent = normalize(bitangent);

	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		frag_UV = geom_UV[i];
		frag_Normal = geom_Normal[i];
		frag_Position = geom_Position[i];
		frag_Tangent = tangent;
		frag_Bitangent = bitangent;
		EmitVertex();
	}
	EndPrimitive();
	
}