#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNor;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 camoff;
uniform float treeoff;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
void main()
{
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
	vec4 tpos =  vec4(vertPos, 1.0);
    
    //use exquation of a circle to get position
    tpos.x += (treeoff/15.0) * sin(treeoff);
    tpos.z += (treeoff/15.0) * cos(treeoff);
      
    tpos = M * tpos;
      
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex = vertTex;
}
