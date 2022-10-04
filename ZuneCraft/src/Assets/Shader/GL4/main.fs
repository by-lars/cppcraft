#version 460 core

in vec2 vTexCoords;
in vec2 vTilePos;
in float vNormal;
out vec4 FragColor;
in float vVisibility;

uniform sampler2D uAtlas;
uniform vec3 uSkyColor;

void main() {
   
    vec2 size = vec2(1.0 / 16.0);

    vec2 coords = vTilePos + size * fract(vTexCoords);

    vec4 albedo = texture2D(uAtlas, coords) * 0.8 + vec4(vec3(vNormal / 10.0), 1.0) * 0.2;
   // FragColor = mix(vec4(uSkyColor, 1.0), albedo, vVisibility);;
   FragColor = albedo;
}