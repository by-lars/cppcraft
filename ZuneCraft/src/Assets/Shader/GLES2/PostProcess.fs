
varying vec2 vTexCoord;
uniform sampler2D uTexture;

void main() {
    gl_FragColor = texture2D(uTexture, vTexCoord) * 0.9 + vec4(vTexCoord.x, vTexCoord.y, 1.0, 1.0) * 0.1;
}