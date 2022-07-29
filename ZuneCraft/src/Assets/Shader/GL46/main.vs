#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aData;
layout(location = 2) in mat4 aModel;

out vec2 vTexCoords;
out vec2 vTilePos;
out float vNormal;

uniform mat4 uView;
uniform mat4 uProj;

void main() {
	//int texX = mask[n].Type % 16;
	//int texY = 15 - mask[n].Type / 16;

	vTexCoords = aData.xy;
	vTilePos = vec2(floor(mod(aData.z - 1.0, 16.0)), 15.0 - floor((aData.z-1.0) / 16.0)) / vec2(16.0);
	vNormal = aData.w;

	gl_Position = uProj * uView * aModel * vec4(aPos, 1.0);
}