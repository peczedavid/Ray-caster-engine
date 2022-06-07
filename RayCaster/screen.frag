#version 460 core
precision highp float;

out vec4 outColor;

in vec2 uv;

uniform sampler2D screen;

void main() {
	outColor = texture2D(screen, uv);
}