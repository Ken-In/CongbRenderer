#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform float exposure;

void main(){
    vec3 hdrCol = texture(screenTexture, TexCoords).rgb;
    vec3 bloomCol = texture(bloomBlur, TexCoords).rgb;
    bloomCol -= hdrCol;
    bloomCol = max(bloomCol, 0.0);
    hdrCol += bloomCol;

    //Exposure tone mapping
    vec3 toneMappedResult = vec3(1.0) - exp(-hdrCol * exposure);

    FragColor = vec4(toneMappedResult, 1.0) ;
}