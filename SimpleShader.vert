#version 330

uniform mat4 M;
layout (location=0) in vec4 position;

void main(){
    gl_Position = M*position;
}


