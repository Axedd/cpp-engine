#version 330 core

layout (location = 0) in vec3 aPos;       // x, y, z
layout (location = 1) in vec2 aTexCoord;  // u, v
layout (location = 2) in vec4 aColor;     // r, g, b, a

out vec4 vertexColor;
out vec2 texCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
    texCoord = aTexCoord;
}