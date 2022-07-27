varying vec2 vTexCoords;
varying vec2 vTilePos;
varying float vNormal;

uniform sampler2D uAtlas;

void main() {
   
    vec2 size = vec2(1.0 / 16.0);

    vec2 coords = vTilePos + size * fract(vTexCoords);

    gl_FragColor = texture2D(uAtlas, coords) * 0.8 + vec4(vec3(vNormal / 10.0), 1.0) * 0.2;
}