#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int shininess;
};

struct DirLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

out vec4 gl_FragColor;

in vec3 v_Position;
in vec3 v_Normal;

uniform Material u_Material;
uniform DirLight u_Light;
uniform mat4 u_View;

void main()
{
    vec3 view = normalize(-v_Position);
    vec3 normal = normalize(v_Normal);
    vec3 light = normalize(mat3(u_View) * -u_Light.direction);
    vec3 reflect = 2.0 * dot(light, normal) * normal - light;

    float lDotn = dot(light, normal);
    float vDotr = dot(view, reflect);

    vec3 ambient = u_Light.ambient * u_Material.ambient;
    vec3 diffuse = u_Light.diffuse * u_Material.diffuse * max(lDotn, 0.0);
    vec3 specular = u_Light.specular * u_Material.specular * pow(max(vDotr, 0.0), u_Material.shininess);

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}