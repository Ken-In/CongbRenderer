#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main(){
    vec3 color = texture(screenTexture, TexCoords).rgb;
    //这里的亮度系数向量 vec3 是根据人眼对不同颜色的敏感度设定的
    //其中 0.2126、0.7152 和 0.0722 分别对应于红色、绿色和蓝色的权重。
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    //high band pass filter
    if( brightness >= 1.0 ){
        FragColor = vec4(color, 1.0);
    }
    else
    {
        FragColor = vec4(vec3(0.0), 1.0);
    }
}