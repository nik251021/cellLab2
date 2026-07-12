#version 330 core
layout(location = 0) in vec2 position;

uniform mat4 u_ViewProj;

void main() {
    gl_Position = u_ViewProj * vec4(position, 0.0, 1.0);
}