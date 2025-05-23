#include "Render3D.h"
#include "ZBuffer.h"
#include "bezier.h"
#include "SimpleShader.h"
#include "transforms.h"
#include <functional>

using Curve = std::function<vec3(float)>;

vec3 coons(float s, float t, Curve C[2], Curve D[2]){
	vec3 Lc = lerp(t, C[0](s), C[1](s));
	vec3 Ld = lerp(s, D[0](t), D[1](t));
	vec3 B = bilerp(s, t, C[0](0), C[0](1), C[1](0), C[1](1));
	return Lc + Ld - B;
}

std::vector<vec3> polygonize_coons_path(vec3 P[4][4], unsigned int m, unsigned int n){
	unsigned int N = m*n;
	float u0 = 0, u1 = 1, du = (u1-u0)/(m-1),
              v0 = 0, v1 = 1, dv = (v1-v0)/(n-1);

	Curve C[] = {
		[&](float t){ return bezier<3>(P[0], t); },
		[&](float t){ return bezier<3>(P[1], t); },
	};
	Curve D[] = {
		[&](float t){ return bezier<3>(P[2], t); },
		[&](float t){ return bezier<3>(P[3], t); },
	};

	std::vector<vec3> V(N);
	for(unsigned int i = 0; i < m; i++){
		for(unsigned int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			unsigned int ij = i + j*m;
			V[ij] = coons(u, v, C, D);
		}
	}
	return V;
}

std::vector<unsigned int> get_indices(unsigned int m, unsigned int n){
	std::vector<unsigned int> indices;
	for(unsigned int i = 0; i < m; i++){
		for(unsigned int j = 0; j < n; j++){	
			unsigned int ij = i + j*m;
			if(i < m-1){
				indices.push_back(ij);
				indices.push_back(ij+1);
			}
			
			if(j < n-1){
				indices.push_back(ij);
				indices.push_back(ij+m);
			}
		}
	}
	return indices;
}

int main(){
	vec3 P1[4][4] = { 
		{{ 1,0,0}, { 1,0,1}, { 0,0,1}, { 0,0,0}},
		{{ 1,2,-2}, { 1,1,-2}, { 0,1,-2}, { 0,2,-2}},
		{{ 1, 0, 0},{ 1, 1, 0}, { 1,2,-1},{ 1,2,-2}},
		{{ 0, 0, 0},{ 0, 1, 0}, { 0, 2, -1},{ 0, 2, -2}},
	};
	
	vec3 P2[4][4] = { 
		{{ -1,0,0}, { -1,0,-1}, { -2,0,-1}, { -2,0,0}},
		{{ -1,2,-2}, { -1,1,-2}, { -2,1,-2}, { -2,2,-2}},
		{{ -1, 0, 0},{ -1, 1, 0}, { -1,2,-1},{ -1,2,-2}},
		{{ -2, 0, 0},{ -2, 1, 0}, { -2, 2, -1},{ -2, 2, -2}},
	};

	unsigned int m = 20, n = 20;
	std::vector<vec3> V1 = polygonize_coons_path(P1, m, n);
	std::vector<vec3> V2 = polygonize_coons_path(P2, m, n);
	std::vector<unsigned int> indices = get_indices(m, n);
	Elements<Lines> L{indices};

	int w = 600, h = 600;
	ImageRGB G{w, h};
	mat4 View = lookAt({5, 5 ,1}, {0, 0, 0}, {0, 0, 1});
	float a = w/(float)h;
	mat4 Projection = perspective(50, a, 0.1, 50);

	SimpleShader shader;
	shader.C = blue;

	int nframes = 30;
	for(int k = 0; k < nframes; ++k){
		G.fill(white);
		ImageZBuffer I{G};

		float theta = k*2*M_PI/nframes;
		mat4 Model = rotate_z(theta);

		shader.M = Projection*View*Model;

		Render3D(V1, L, shader, I);
		Render3D(V2, L, shader, I);

		G.save_frame(k, "anim/output", "png");
	}
}
