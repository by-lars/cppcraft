varying vec2 vTexCoords;
varying vec2 vTilePos;
varying float vNormal;
varying float vVisibility;

uniform sampler2D uAtlas;
uniform vec3 uSkyColor;

void main() {
   
    vec2 size = vec2(1.0 / 16.0);

    vec2 coords = vTilePos + size * fract(vTexCoords);

    vec4 albedo = texture2D(uAtlas, coords) * 0.5 + vec4(vec3(vNormal / 10.0), 1.0) * 0.5;
    
    gl_FragData[0] = mix(vec4(uSkyColor, 1.0), albedo, vVisibility);
}