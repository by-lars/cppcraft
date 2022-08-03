attribute vec4 aPos;
attribute vec4 aData;

varying vec2 vTexCoords;
varying vec2 vTilePos;
varying float vNormal;

uniform vec3 uTranslation[32];
uniform mat4 uView;
uniform mat4 uProj;

void main() {
	vTexCoords = aData.xy;
	vTilePos = vec2(floor(mod(aPos.w - 1.0, 16.0)), 15.0 - floor((aPos.w-1.0) / 16.0)) / vec2(16.0);
	vNormal = aData.z;

	gl_Position = uProj * uView * vec4(aPos.xyz + uTranslation[aData.w], 1.0);
}