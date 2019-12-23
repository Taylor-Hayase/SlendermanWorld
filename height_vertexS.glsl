#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 camoff;

out vec4 view_pos;

void main()
{
	vec4 tpos =  vec4(vertPos, 1.0);
    
    tpos.z -=camoff.z;
    tpos.x -=camoff.x;
    
	tpos =  M * tpos;
    view_pos = V * tpos;
	gl_Position = P * V * tpos;

}
