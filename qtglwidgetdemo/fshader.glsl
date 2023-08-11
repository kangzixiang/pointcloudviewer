#version 310 es
#undef lowp
#undef mediump
#undef highp
precision mediump float;

in vec3 inColor;
out vec4 outColor;

void main()
{
    outColor = vec4(inColor, 1.0);
}