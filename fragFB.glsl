#version 410 core
out vec4 color;
in vec2 fragTex;
  
uniform sampler2D tex;
uniform sampler2D shadowMapTex;

void main()
{
    vec3 texturecolor;
    texturecolor = texture(tex, fragTex).rgb;

    color.rgb = texturecolor;
	
    color.a=1;
}
