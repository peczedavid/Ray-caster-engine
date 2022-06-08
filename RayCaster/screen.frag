#version 460 core
precision highp float;

out vec4 bufferPixelColor;

in vec2 uv;

uniform sampler2D u_screenTexture;

void main() {
	bufferPixelColor = texture(u_screenTexture, uv);
}