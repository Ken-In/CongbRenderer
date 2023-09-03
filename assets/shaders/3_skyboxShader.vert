#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 VP;

void main(){
    TexCoords = aPos;
    vec4 pos = VP * vec4(aPos, 1.0);
    // 因为 vertex shader 后会进行一次透视除法 xyz/w， 将 z 设为 w， 即可使 z 一直为 1保持在远平面
    gl_Position = pos.xyww;
}