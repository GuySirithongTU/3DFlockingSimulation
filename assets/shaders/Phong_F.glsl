#version 330 core

in vec3 v_Color;

out vec4 gl_FragColor;

void main()
{
    gl_FragColor = vec4(v_Color, 1.0);
}