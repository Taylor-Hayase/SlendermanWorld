#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 vertex_normal;


uniform sampler2D tex7;
uniform vec3 camoff;
uniform vec3 campos;

//ground
void main()
{
    vec3 n = normalize(vertex_normal);
    vec3 lp=vec3(10,-20,-100);
    vec3 ld = normalize(vertex_pos - lp);
    float diffuse = dot(n,ld);
    
    color.rgb = texture(tex7, vertex_tex*50).rgb;
    
    color.rgb *= diffuse* 0.7;
    color.a=1;

    float len = length(vertex_pos.xz+campos.xz);
    len-=41;
    len/=8.;
    len=clamp(len,0,1);
    color.a=1-len;


}
