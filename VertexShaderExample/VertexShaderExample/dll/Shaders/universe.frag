#version 430
in vec2 frag_UV;

out vec4 frag_color;

uniform sampler2D Universe;

void main() {
    frag_color = texture(Universe, frag_UV);
}
