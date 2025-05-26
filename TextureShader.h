#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "Sampler2D.h"

struct TextureShader{
	struct Varying{
		vec4 position;
		vec2 texCoords;
	};

	mat4 M;
	Sampler2D texture;

	template<class Vertex>
	void vertexShader(Vertex in, Varying& out){
		out.position = M*getPosition(in);
		out.texCoords = in.texCoords;
	}

	void fragmentShader(Varying V, RGB& FragColor){
		FragColor = texture.sample(V.texCoords);
	}
};

#endif
