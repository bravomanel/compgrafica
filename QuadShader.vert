#version 330

out vec2 texCoords;

vec2 V[4] = vec2[](
	vec2(0, 0),
	vec2(1, 0),
	vec2(1, 1),
	vec2(0, 1)
);

void main(){
	vec2 v = V[gl_VertexID];
	gl_Position = vec4(2*v-1, 0, 1);
	
	texCoords = v;
}
