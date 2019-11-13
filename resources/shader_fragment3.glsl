#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D tex4;
uniform sampler2D tex5;


//slenderman shader
void main()
{
vec3 n = normalize(vertex_normal);
vec3 lp=vec3(10,-20,-100);
vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);

    
//clothes texture
color.rgb = texture(tex4, vertex_tex).rgb;
    
//white head
if (vertex_pos.y > -0.2) {
    color.rgb = vec3(1, 1, 1);
}
//color += texture(tex5, vertex_tex).rgb;


//color = vec3(0.3, 0.3, 0.3);

color.rgb *= diffuse*0.7;

vec3 cd = normalize(vertex_pos - campos);
vec3 h = normalize(cd+ld);
float spec = dot(n,h);
spec = clamp(spec,0,1);
spec = pow(spec,20);

//color += vec3(1,1,1)*spec*5;
    color.a = 1;

}
