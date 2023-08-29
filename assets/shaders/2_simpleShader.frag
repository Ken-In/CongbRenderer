#version 430 core

out vec4 FragColor;

in VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    vec4 fragPos_lS;
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D emissiveMap;
uniform sampler2D normalsMap;
uniform sampler2D lightMap;
uniform sampler2D metalRoughMap;
uniform sampler2D shadowMap;

uniform vec3 cameraPos_wS;

struct PointLight{
    vec4 position;
    vec4 color;
    bool enabled;
    float intensity;
    float range;
};

// point light SSBO
layout (std430, binding = 3) buffer lightSSBO{
    PointLight pointLight[];
};

// point light shadow map
#define SHADOW_CASTING_POINT_LIGHTS 4
uniform float far_plane;
uniform int light_count;
uniform samplerCube depthMaps[SHADOW_CASTING_POINT_LIGHTS];

// directional light
struct DirLight
{
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

vec3 sampleOffsetDirections[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

// functions
float calcDirShadow(vec4 fragPosLightSpace);
vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float viewDistance);
float calcPointLightShadows(samplerCube depthMap, vec3 fragPos, float viewDistance);

void main() {
    vec4 color      = texture(albedoMap, fs_in.texCoords).rgba;
    vec3 albedo     = color.rgb;
    float alpha     = color.a;
    vec3 norm       = normalize(fs_in.N);
    
    vec3 lightDir   = normalize(dirLight.direction);
    vec3 viewDir    = normalize(cameraPos_wS - fs_in.fragPos_wS);
    float viewDistance = length(cameraPos_wS - fs_in.fragPos_wS);
    float shadow    = calcDirShadow(fs_in.fragPos_lS);
    vec3 radianceOut = vec3(0.0);
    radianceOut += albedo * (1.0f - shadow);
    for(uint i = 0; i < light_count; i++)
    {
        radianceOut += calcPointLight(i, norm, fs_in.fragPos_wS, viewDir, albedo, viewDistance);
    }
    
    // ambient 
    radianceOut += albedo * 0.05f;
    FragColor = vec4(radianceOut, 1.0);
}

//Sample offsets for the pcf are the same for both dir and point shadows
float calcDirShadow(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = 0.0;
    int   samples = 9;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int i = 0; i < samples; ++i){
        float pcfDepth = texture(shadowMap, projCoords.xy + sampleOffsetDirections[i].xy * texelSize).r;
        shadow += projCoords.z - bias > pcfDepth ? 0.111111 : 0.0;
    }

    return shadow;
}

vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos, vec3 viewDir,
vec3 albedo, float viewDistance)
{
    //Point light basics
    vec3 position = pointLight[index].position.xyz;
    vec3 color    = 100.0 * pointLight[index].color.rgb;
    float radius  = pointLight[index].range;

    //Stuff common to the BRDF subfunctions 
    vec3 lightDir = normalize(position - fragPos);
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);

    //Attenuation calculation that is applied to all
    float distance    = length(position - fragPos);
    float attenuation = pow(clamp(1 - pow((distance / radius), 4.0), 0.0, 1.0), 2.0)/(1.0  + (distance * distance));
    vec3 radianceIn   = color * attenuation;

    //shadow stuff
    vec3 lightToFrag = fragPos - position;
    float shadow = calcPointLightShadows(depthMaps[index], lightToFrag, viewDistance);

    vec3 radiance = radianceIn * nDotL * albedo * (1 - shadow);

    return radiance;
}

float calcPointLightShadows(samplerCube depthMap, vec3 fragToLight, float viewDistance){
    float shadow      = 0.0;
    float bias        = 0.0;
    int   samples     = 8;
    float fraction    = 1.0/float(samples);
    float diskRadius  = (1.0 + (viewDistance / far_plane)) / 25.0;
    float currentDepth = (length(fragToLight) - bias);

    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(depthMap, fragToLight + (sampleOffsetDirections[i] * diskRadius)).r;
        closestDepth *= far_plane;
        if(currentDepth > closestDepth){
            shadow += fraction;
        }
    }
    return shadow;
}





