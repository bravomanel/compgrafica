#ifndef MIX_COLOR_SHADER_H
#define MIX_COLOR_SHADER_H

#include "ColorShader.h"

struct MixColorShader{
	using Varying = ColorShader::Varying;

	mat4 M;
	vec3 pmin, pmax;
	std::array<RGB, 8> C;
	
	template<typename Vertex>
	void vertexShader(Vertex in, Varying& out){
		vec4 pos = getPosition(in);
		out.position = M*pos;

		vec3 p = toVec3(pos);
		vec3 d = (p-pmin)/(pmax - pmin);
		float dx = d[0], dy = d[1], dz = d[2];

		RGB C1 = bilerp(dx, dz, C[0], C[1], C[2], C[3]);
		RGB C2 = bilerp(dx, dz, C[4], C[5], C[6], C[7]);
		out.color = toVec(lerp(dy, C1, C2));
	}

	void fragmentShader(Varying V, RGB& FragColor){
		FragColor = toColor(V.color);
	}
};

#endif
