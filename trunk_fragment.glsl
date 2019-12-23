#version 330 core
out vec4 color;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 vfrag;
in vec3 view_normal;
in vec4 fragLightSpacePos;


uniform vec3 campos;

uniform sampler2D tex; //framebuffer texture
uniform sampler2D texTrunk; //trunk tecture
uniform sampler2D shadowMapTex; //shadowmap texture

float calcShadowFactor(vec4 lightSpacePosition) {
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

    float shadowFactor = 0;
    float bias = 0.01;
    float fragDepth = shifted.z - bias;
    
    const int numSamples = 9;

    vec2 texelSize = 1.0 / textureSize(tex, 0);
    
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMapTex, shifted.xy + vec2(x, y) * texelSize).r;
            shadowFactor += fragDepth >= pcfDepth ? 1.0 : 0.0;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
    
}

//tree trunk shader
void main()
{
    
    color.rgb = texture(texTrunk, vertex_tex).rgb;

    vec3 n = normalize(view_normal);
   // color = vec4(n, 1.0);
    
    //return;
    vec3 ld = vec3(0, 0, -1);
    vec3 lp = -campos;
    vec3 vd = normalize(vfrag);
    float len = length(vertex_pos.xz+campos.xz);
    
    float diffuse = dot(n,-ld);
    color.rgb *= diffuse;
    
    //lighting
    float inner_cutoff = 0.97;
    float outer_cutoff = 0.90;
    float theta = dot(vd, ld);
    float epsilon = inner_cutoff - outer_cutoff;
    
    //flashlight
    if (theta > inner_cutoff)
    {
        color.rgb *= 0.7;
        color.a = 1;
    }
    //fade off
    else if (theta > outer_cutoff) {
        color.rgb *= ((theta - outer_cutoff)/epsilon) - 0.3;
    }
    //outside flashlight
    else
    {
        color.rgb *= 0.01;
        float dist = length(vfrag);
        color.rgb *= (8 - (dist));
        color.a = 1;
    }

    //distance fade
    color.rgb *= (1 - (len / 30));
    len-=42;
    len/=9.;
    len=clamp(len,0,1);
    color.a=1-len;

}
