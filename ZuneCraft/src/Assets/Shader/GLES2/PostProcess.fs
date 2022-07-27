
varying vec2 vTexCoord;
uniform sampler2D uTexture;

void main() {
    gl_FragColor = texture2D(uTexture, vTexCoord) + vec4(vTexCoord.x, vTexCoord.y, 1.0, 1.0);
}