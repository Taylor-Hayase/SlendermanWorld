#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 vertex_normal;
in vec3 vfrag;
in vec4 fragLightSpacePos;

uniform vec3 camoff;
uniform vec3 campos;

uniform sampler2D tex; //framebuffer
uniform sampler2D texGrass; //grass
uniform sampler2D shadowMapTex; //shadowmap

float calcShadowFactor(vec4 lightSpacePosition) {
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

	float shadowFactor = 0;
    float bias =0.00001;
    float fragDepth = shifted.z - bias;

    const int numSamples = 9;

    vec2 texelSize = 1.0 / textureSize(shadowMapTex, 0);
    
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMapTex, shifted.xy + vec2(x, y) * texelSize).x;
            shadowFactor += fragDepth >= pcfDepth ? 1.0 : 0.0;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
    
}
//ground
void main()
{
    vec2 texcoords = vertex_tex;
    float t = 1./100.;
    texcoords -= vec2(camoff.x, camoff.z)*t;
    
    color.rgb = texture(texGrass, texcoords*50).rgb;
        
    vec3 n = normalize(vertex_normal);
    vec3 ld = vec3(0, 0, -1);
    vec3 vd = normalize(vfrag);
    float len = length(vertex_pos.xz+campos.xz);
        
    //lighting
    float inner_cutoff = 0.95;
    float outer_cutoff = 0.88;
    float theta = dot(vd, ld);
    float epsilon = inner_cutoff - outer_cutoff;
        
    //flashlight
    //***********************************************
    if (theta > inner_cutoff && theta >= outer_cutoff)
    {
        color.rgb *= 0.7;
        color.a = 1;
    }
    //outside flashlight
    else if (theta < outer_cutoff) {
        float diffuse = dot(n,-ld);
        color.rgb *= diffuse;
        color.a = 1;
    }
    //fade off
    else {
        float factor = ((theta - outer_cutoff)/epsilon) - 0.3;
        color.rgb *= clamp(factor, 0, 0.65);
        color.a = 1;
    }
    //************************************************
    //distance fade
    
    color.rgb *= (1 - (len / 30));
    
    len-=41;
    len/=8.;
    len=clamp(len,0,1);
    color.a=1-len;
        
     //shadow stuff here
    float shadowFactor = 1. - calcShadowFactor(fragLightSpacePos);
    color.rgb *= abs(shadowFactor);

}
