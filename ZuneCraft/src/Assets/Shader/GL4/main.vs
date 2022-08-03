#version 460 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aData;
layout(location = 2) in vec3 aTranslation;

out vec2 vTexCoords;
out vec2 vTilePos;
out float vNormal;

uniform mat4 uView;
uniform mat4 uProj;

void main() {
	vTexCoords = aData.xy;
	vTilePos = vec2(floor(mod(aPos.w - 1.0, 16.0)), 15.0 - floor((aPos.w-1.0) / 16.0)) / vec2(16.0);
	vNormal = aData.z;

	gl_Position = uProj * uView * vec4(aPos.xyz + aTranslation, 1.0);
}