#version 330 core

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoord;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_TexCoord = a_Position;
    gl_Position = (u_Projection * u_View * vec4(a_Position, 1.0)).xyww;
}