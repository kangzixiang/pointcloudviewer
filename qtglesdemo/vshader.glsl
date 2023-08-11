#version 310 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 inColor;
uniform mat4 MVP;

void main()
{
    inColor = aColor;
    gl_Position = MVP * vec4(aPos, 1.0);
}