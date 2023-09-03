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

// IBL textures
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform float zFar;
uniform float zNear;
uniform vec3 cameraPos_wS;

// directional light
struct DirLight
{
    vec3 direction;
    vec3 color;
};
uniform DirLight dirLight;

// point light
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
#define M_PI 3.1415926535897932384626433832795
uniform float far_plane;
uniform int maxLightsPerTile;
uniform samplerCube depthMaps[SHADOW_CASTING_POINT_LIGHTS];

// cluster
struct LightGrid{
    uint offset;
    uint count;
};
layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uint tileSizeX;
    uint tileSizeY;
    uint tileSizeZ;
    uint padding1;
    vec2 tileSizePx;
    vec2 viewPxSize;
    float scale;
    float bias;
    uint padding2;
    uint padding3;
};
layout (std430, binding = 4) buffer lightIndexSSBO{
    uint globalLightIndexList[];
};
layout (std430, binding = 5) buffer lightGridSSBO{
    LightGrid lightGrid[];
};

vec3 sampleOffsetDirections[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec3 colors[8] = vec3[](
vec3(0, 0, 0),    vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1,  1),
vec3(1,  0,  0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);

uniform bool normalMapped;
uniform bool aoMapped;
uniform bool IBL;
uniform bool slices;
uniform bool clusters;

// functions
float calcDirShadow(vec4 fragPosLightSpace);
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float rough, float metal, float shadow, vec3 F0);
vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float rough, float metal, vec3 F0,  float viewDistance);
float calcPointLightShadows(samplerCube depthMap, vec3 lightToFrag, float viewDistance);

// PBR functions
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float distributionGGX(vec3 N, vec3 H, float rough);
float geometrySchlickGGX(float nDotV, float rough);
float geometrySmith(float nDotV, float nDotL, float rough);

float linearDepth(float depthSample);

void main() {
    vec4 color      =  texture(albedoMap, fs_in.texCoords).rgba;
    vec3 emissive   =  texture(emissiveMap, fs_in.texCoords).rgb;
    float ao        =  texture(lightMap, fs_in.texCoords).r;
    vec2 metalRough =  texture(metalRoughMap, fs_in.texCoords).bg;
    float metallic  =  metalRough.x;
    float roughness =  metalRough.y;
    
    vec3 albedo     = color.rgb;
    float alpha     = color.a;
    if(alpha < 0.5){
        discard;
    }
    
    // 法线从normalMap或顶点数据得到
    vec3 norm = vec3(0.0);
    if(normalMapped){
        vec3 normal = normalize(2.0 * texture(normalsMap, fs_in.texCoords).rgb - 1.0);
        mat3 TBN  = mat3(fs_in.T, fs_in.B, fs_in.N);
        norm = normalize(TBN * normal );
    }
    else{
        norm = normalize(fs_in.N);
    }
    
    // bacis direction
    vec3 lightDir   = normalize(dirLight.direction);
    vec3 viewDir    = normalize(cameraPos_wS - fs_in.fragPos_wS);
    vec3 reflectDir = reflect(-viewDir, norm);
    
    // fresnel
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    vec3 radianceOut = vec3(0.0);
    
    // directional light radiance
    float shadow    = calcDirShadow(fs_in.fragPos_lS);
    radianceOut += calcDirLight(dirLight, norm, viewDir, albedo, roughness, metallic, shadow, F0) ;
    
    // point light radiance
    uint zTile     = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0.0));
    uvec3 tiles    = uvec3( uvec2( gl_FragCoord.xy * tileSizePx ), zTile);
    uint tileIndex = tiles.x +
                     tileSizeX * tiles.y +
                     (tileSizeX * tileSizeY) * tiles.z;
    
    uint lightCount         = lightGrid[tileIndex].count;
    uint lightIndexOffset   = lightGrid[tileIndex].offset;

    float viewDistance = length(cameraPos_wS - fs_in.fragPos_wS);
    
    for(uint i = 0; i < lightCount; i++)
    {
        uint lightVectorIndex = globalLightIndexList[lightIndexOffset + i];
        radianceOut += calcPointLight(lightVectorIndex, norm, fs_in.fragPos_wS, viewDir, albedo, roughness, metallic, F0, viewDistance);
    }
    
    // ambient 
    vec3 ambient = vec3(0.025) * albedo;
    if(IBL)
    {
        vec3  kS = fresnelSchlickRoughness(max(dot(norm, viewDir), 0.0), F0, roughness);
        vec3  kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        vec3 irradiance = texture(irradianceMap, norm).rgb;
        vec3 diffuse    = irradiance * albedo;

        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, reflectDir, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF = texture(brdfLUT, vec2(max(dot(norm, viewDir), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
        ambient = (kD * diffuse + specular);
    }
    if(aoMapped)
    {
        ambient *= ao;
    }
    radianceOut += ambient;
    
    radianceOut += emissive;
    
    if(slices){
        FragColor = vec4(colors[uint(mod(float(zTile), 8.0))], 1.0);
    }
    else if(clusters)
    {
        float factor = lightCount / float(maxLightsPerTile);
        factor = clamp(factor, 0.0, 1.0);
        FragColor = mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), factor);
    }
    else{
        FragColor = vec4(radianceOut, 1.0);
    }
    
}

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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float rough, float metal, float shadow, vec3 F0)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);
    vec3 radianceIn = dirLight.color;

    //Cook-Torrance BRDF
    float NDF = distributionGGX(normal, halfway, rough);
    float G   = geometrySmith(nDotV, nDotL, rough);
    vec3  F   = fresnelSchlick(max(dot(halfway,viewDir), 0.0), F0);

    //Finding specular and diffuse component
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metal;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular = numerator / max (denominator, 0.0001);

    vec3 radiance = (kD * (albedo / M_PI) + specular ) * radianceIn * nDotL;
    radiance *= (1.0 - shadow);

    return radiance;
}

vec3 calcPointLight(uint index, vec3 normal, vec3 fragPos,
vec3 viewDir, vec3 albedo, float rough, float metal, vec3 F0,  float viewDistance)
{
    // 点光源信息
    vec3 position = pointLight[index].position.xyz;
    vec3 color    = 100.0 * pointLight[index].color.rgb;
    float radius  = pointLight[index].range;

    // BRDF 参数 
    vec3 lightDir = normalize(position - fragPos);
    vec3 halfway  = normalize(lightDir + viewDir);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotL = max(dot(normal, lightDir), 0.0);

    // 衰减
    float distance    = length(position - fragPos);
    float attenuation = pow(clamp(1 - pow((distance / radius), 4.0), 0.0, 1.0), 2.0)/(1.0  + (distance * distance));
    vec3 radianceIn   = color * attenuation;

    // 光源到像素
    vec3 lightToFrag = fragPos - position;
    // 超过4个的点光不计算阴影
    float shadow = index >= SHADOW_CASTING_POINT_LIGHTS ? 0 : calcPointLightShadows(depthMaps[index], lightToFrag, viewDistance);

    vec3 radiance = radianceIn * nDotL * albedo * (1 - shadow);

    return radiance;
}

float calcPointLightShadows(samplerCube depthMap, vec3 lightToFrag, float viewDistance){
    float shadow      = 0.0;
    float bias        = 0.0;
    int   samples     = 8;
    float fraction    = 1.0/float(samples);
    float diskRadius  = (1.0 + (viewDistance / far_plane)) / 25.0;
    float currentDepth = (length(lightToFrag) - bias);

    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(depthMap, lightToFrag + (sampleOffsetDirections[i] * diskRadius)).r;
        closestDepth *= far_plane;
        if(currentDepth > closestDepth){
            shadow += fraction;
        }
    }
    return shadow;
}

float linearDepth(float depthSample)
{
    float depthRange = 2.0 * depthSample - 1.0;
    float linear = (2.0 * zNear * zFar) / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}

// PBR functions
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    float val = 1.0 - cosTheta;
    return F0 + (1.0 - F0) * (val*val*val*val*val); //Faster than pow
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    float val = 1.0 - cosTheta;
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * (val*val*val*val*val); //Faster than pow
}

float distributionGGX(vec3 N, vec3 H, float rough){
    float a  = rough * rough;
    float a2 = a * a;

    float nDotH  = max(dot(N, H), 0.0);
    float nDotH2 = nDotH * nDotH;

    float num = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = 1 / (M_PI * denom * denom);

    return num * denom;
}

float geometrySchlickGGX(float nDotV, float rough){
    float r = (rough + 1.0);
    float k = r*r / 8.0;

    float num = nDotV;
    float denom = 1 / (nDotV * (1.0 - k) + k);

    return num * denom;
}

float geometrySmith(float nDotV, float nDotL, float rough){
    float ggx2  = geometrySchlickGGX(nDotV, rough);
    float ggx1  = geometrySchlickGGX(nDotL, rough);

    return ggx1 * ggx2;
}


