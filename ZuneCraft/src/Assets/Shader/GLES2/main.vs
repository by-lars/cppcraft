attribute vec4 aPos;
attribute vec4 aData;

varying vec2 vTexCoords;
varying vec2 vTilePos;
varying float vNormal;
varying float vVisibility;

uniform vec3 uBatchData[128];
uniform mat4 uView;
uniform mat4 uProj;

const float density = 0.005;
const float gradient = 5.0;

void main() {
	vTexCoords = aData.xy;
	vTilePos = vec2(floor(mod(aPos.w - 1.0, 16.0)), 15.0 - floor((aPos.w-1.0) / 16.0)) / vec2(16.0);
	vNormal = aData.z;

	vec4 viewSpacePos = uView * vec4(aPos.xyz + uBatchData[aData.w], 1.0);

	float distance = length(viewSpacePos.xyz);
	vVisibility = exp(-pow((distance*density), gradient));
	vVisibility = clamp(vVisibility, 0.0, 1.0);

	gl_Position = uProj * viewSpacePos;
}