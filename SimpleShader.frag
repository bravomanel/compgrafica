#version 330

uniform vec3 C;
out vec4 fragColor;

void main(){
    float x = 4.8;
    fragColor = vec4(C, 1);
}
