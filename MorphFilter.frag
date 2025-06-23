#version 330

uniform sampler2D tex;

in vec2 texCoords;

out vec4 fragColor;

float texel(ivec2 pos, int i, int j){
	return texelFetch(tex, pos + ivec2(i, j), 0).r;
}

bool erosion(bool c[5]){
	return c[0] && c[1] && c[2] && c[3] && c[4];
}

bool dilation(bool c[5]){
	return c[0] || c[1] || c[2] || c[3] || c[4];
}

void main(){
	ivec2 pos = ivec2(gl_FragCoord.xy);
	bool c[5] = bool[5](
        texel(pos, 0, 0) > 0, 
        texel(pos, 1, 0) > 0, 
        texel(pos,-1, 0) > 0, 
        texel(pos, 0, 1) > 0, 
        texel(pos, 0,-1) > 0
    );

	fragColor = vec4(0, 0, 0, 1);

    //if(dilation(c))
    if(erosion(c))
		fragColor = vec4(1, 1, 1, 1);
}
