attribute vec3 aPos;
attribute vec4 aData;

varying vec2 vTexCoords;
varying vec2 vTilePos;
varying float vNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main() {
	//int texX = mask[n].Type % 16;
	//int texY = 15 - mask[n].Type / 16;

	vTexCoords = aData.xy;
	vTilePos = vec2(floor(mod(aData.z - 1.0, 16.0)), 15.0 - floor((aData.z-1.0) / 16.0)) / vec2(16.0);
	vNormal = aData.w;

	gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}