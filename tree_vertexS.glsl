#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 camoff;
uniform float treeoff;

void main()
{
	vec4 tpos =  vec4(vertPos, 1.0);
    
    //use exquation of a circle to get position
    tpos.x += (treeoff/15.0) * sin(treeoff);
    tpos.z += (treeoff/15.0) * cos(treeoff);
      
    tpos = M * tpos;

	gl_Position = P * V * tpos;

}
