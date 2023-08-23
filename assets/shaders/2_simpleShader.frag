#version 430 core

out vec4 FragColor;

in VS_OUT{
    vec3 fragPos_wS;
    vec2 texCoords;
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

uniform sampler2D albedoMap;

void main() {
    FragColor = texture(albedoMap, fs_in.texCoords).rgba;
}
