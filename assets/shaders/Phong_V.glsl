#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

out vec3 v_Position;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_NormalMat;

void main()
{
    v_Position = (u_View * u_Model * vec4(a_Position, 1.0)).xyz;
    v_Normal = (u_NormalMat * vec4(normalize(a_Normal), 0.0)).xyz;
    gl_Position = u_Projection * vec4(v_Position, 1.0);
}