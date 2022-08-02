#version 460 core

in vec2 vTexCoord;
uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture2D(uTexture, vTexCoord) * 0.9 + vec4(vTexCoord.x, vTexCoord.y, 1.0, 1.0) * 0.1;
}
