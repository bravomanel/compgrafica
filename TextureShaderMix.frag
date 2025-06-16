#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 texCoords;

out vec4 fragColor;

void main(){
	vec4 c0 = texture(texture0, texCoords);
	vec4 c1 = texture(texture1, texCoords);
	float t = texCoords.s;
	t = t*t*(-2*t + 3);
	fragColor = mix(c0, c1, t);
}
