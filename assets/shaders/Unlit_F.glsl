#version 330 core

out vec4 gl_FragColor;

uniform vec3 u_Color;

void main()
{
    gl_FragColor = vec4(u_Color, 1.0);
}