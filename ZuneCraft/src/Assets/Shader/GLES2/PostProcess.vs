attribute vec4 aPos;
varying vec2 vTexCoord;

uniform int uFlip;

void main() {
	gl_Position = vec4((aPos.x * 2.0 - 1.0) * float(uFlip), aPos.y * 2.0 - 1.0, 0.0, 1.0);
	vTexCoord = vec2(aPos.z, aPos.w);
}
