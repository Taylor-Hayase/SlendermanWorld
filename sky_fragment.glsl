#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D texSky;

//skybox
void main()
{
vec3 n = normalize(vertex_normal);
vec3 lp = -campos;
vec3 ld = normalize(lp - vertex_pos);
float diffuse = dot(n,ld);

color.rgb = texture(texSky, vertex_tex).rgb;
    color.rgb += vec3(0.01, 0.01, 0.01);

vec3 cd = normalize(vertex_pos - campos);
vec3 h = normalize(cd+ld);
float spec = dot(n,h);
spec = clamp(spec,0,1);
spec = pow(spec,20);

    color.a = 1;
}
