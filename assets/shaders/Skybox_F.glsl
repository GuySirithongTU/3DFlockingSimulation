#version 330 core

out vec4 gl_FragColor;

in vec3 v_TexCoord;

uniform samplerCube u_Skybox;

void main()
{
    gl_FragColor = texture(u_Skybox, v_TexCoord);
}