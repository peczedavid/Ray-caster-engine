#version 460 core
precision highp float;

layout(location = 0) in vec2 in_position;
layout(location = 2) in vec2 in_uv;

out vec2 uv;

void main() {
	uv = in_uv;
	gl_Position = vec4(in_position, 0, 1);
}