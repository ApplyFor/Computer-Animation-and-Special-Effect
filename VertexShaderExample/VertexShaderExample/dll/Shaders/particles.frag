#version 430

in float frag_life;
out vec4 outColor;

void main(){
	if (frag_life > 0.4)
	{
		outColor = vec4 (1.0, 0.0, 0.0, 0.2);
	}
	else
	{
		outColor = vec4 (1.0, 1.0, 0.0, 0.2);
	}
}