#version 330 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vec4 position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    position.z = 0.0f;
    gl_Position = position;
}