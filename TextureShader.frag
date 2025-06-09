#version 420

uniform sampler2D myTexture;
uniform bool has_texture = true;
uniform vec3 default_color;

in vec2 texCoords;

out vec4 fragColor;

void main(){
	if(has_texture)
		fragColor = texture(myTexture, texCoords);
	else
		fragColor = vec4(default_color, 1);
}
