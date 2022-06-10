#version 460 core
precision highp float;

out vec4 bufferPixelColor;

in vec2 uv;

uniform sampler2D u_screenTexture;

void main() {
	vec4 pixelColor = texture(u_screenTexture, uv);
	float distance = length(uv * 2.f - 1.f);

	float intensity = 0.4f;

	pixelColor *= (1.f - distance * intensity);
	pixelColor.w = 1.f;

	bufferPixelColor = pixelColor;
}