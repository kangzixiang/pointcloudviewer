#version 310 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 inColor;

uniform mat4 projMatrix;
uniform mat4 worldMatrix;

void main()
{
    inColor = aColor;
    gl_Position = projMatrix * worldMatrix * vec4(aPos, 1.0);
}