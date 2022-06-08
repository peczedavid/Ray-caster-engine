#version 460 core
precision highp float;

layout(location = 0) in vec2 position;

void main() {
	gl_Position = vec4(position, 0, 1);
}