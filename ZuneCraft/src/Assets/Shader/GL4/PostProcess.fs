#version 460 core

in vec2 vTexCoord;
uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture2D(uTexture, vTexCoord);
}
