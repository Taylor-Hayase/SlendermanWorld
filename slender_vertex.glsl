#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNor;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 PVl;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
out vec3 vfrag;
out vec3 view_normal;
out vec4 fragLightSpacePos;


void main()
{
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
    view_normal = vec4(V * vec4(vertex_normal,0.0)).xyz;
	vec4 tpos =  M * vec4(vertPos, 1.0);
    
    fragLightSpacePos = PVl * M * tpos;
    
	vertex_pos = tpos.xyz;
    vfrag = (V * tpos).xyz;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;
}