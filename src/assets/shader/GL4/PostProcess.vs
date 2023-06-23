#version 460 core
layout(location = 0) in vec4 aPos;

out vec2 vTexCoord;

uniform int uFlip;

void main() {
	gl_Position = vec4((aPos.x * 2.0 - 1.0) * float(uFlip), aPos.y * 2.0 - 1.0, 0.0, 1.0);
	vTexCoord = aPos.zw;
}
