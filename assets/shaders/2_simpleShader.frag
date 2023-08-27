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

//TODO:: Probably should be a buffer...
vec3 sampleOffsetDirections[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

// dirLight
struct DirLight
{
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

float calcDirShadow(vec4 fragPosLightSpace);

void main() {
    vec3 norm = normalize(fs_in.N);
    vec3 lightDir = normalize(dirLight.direction);
    float shadow = calcDirShadow(fs_in.fragPos_lS);
    FragColor = texture(albedoMap, fs_in.texCoords).rgba * (1.0f - shadow);
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