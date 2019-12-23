#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec3 vfrag;
in vec3 view_normal;
in vec4 fragLightSpacePos;

uniform vec3 campos;

uniform sampler2D tex; //frambeuffer
uniform sampler2D texSlendy; //slendy
//uniform sampler2D tex5;
uniform sampler2D shadowMapTex; //shadowmap

float calcShadowFactor(vec4 lightSpacePosition) {
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

    float shadowFactor = 0;
    float bias =0.01;
    float fragDepth = shifted.z - bias;

    if (fragDepth > 1.0)
        return 0.0;

    const int numSamples = 9;

    vec2 texelSize = 1.0 / textureSize(tex, 0);
    
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMapTex, shifted.xy + vec2(x, y) * texelSize).r;
            shadowFactor += fragDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
    
}

//slenderman shader
void main()
{
    //clothes texture
    color.rgb = texture(texSlendy, vertex_tex).rgb;
    
    //white head
    if (vertex_pos.y > -0.29) {
        color.rgb = vec3(1, 1, 1);
    }

    vec3 n = normalize(view_normal);
    vec3 ld = vec3(0, 0, -1); //point in front of you
    vec3 vd = normalize(vfrag);
    float len = length(vertex_pos.xz+campos.xz);
    
    float diffuse = dot(n,-ld);
    color.rgb *= diffuse;
    
    //lighting
    float inner_cutoff = 0.95;
    float outer_cutoff = 0.88;
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
    //everything else outside
    else
    {
        color.rgb *= diffuse * 0.01;
        float dist = length(vfrag);
        color.rgb *= (1 - (0.1 *dist));
        color.a = 1;
    }
    
    //distance fade
    color.rgb *= (1 - (len / 30));
    len-=42;
    len/=9.;
    len=clamp(len,0,1);
    color.a=1-len;

}
