#include "Render3D.h"
#include "ZBuffer.h"
#include "bezier.h"
#include "SimpleShader.h"
#include "transforms.h"

vec3 bezier_surface(float u, float v, const std::vector<vec3>& P){
	vec3 Q[] = {
		bezier<3>(P.data(), u),
		bezier<3>(P.data()+4, u),
		bezier<3>(P.data()+8, u),
		bezier<3>(P.data()+12, u)
	};
	return bezier<3>(Q, v);
}

int main(){
	std::vector<vec3> P = {
		{-3, -3, 0}, {-1, -3, 1}, {1, -3, 2}, {3, -3, -1},
		{-3, -1, 1}, {-1, -1, 2}, {1, -1, 0}, {3, -1, 1},
		{-3,  1,-1}, {-1,  1, 1}, {1,  1, 0}, {3,  1, 0},
		{-3,  3, 0}, {-1,  3,-1}, {1,  3, 1}, {3,  3, 1},
	};

	unsigned int m = 50, n = 50, N = m*n;

	float u0 = 0, u1 = 1, du = (u1-u0)/(m-1),
              v0 = 0, v1 = 1, dv = (v1-v0)/(n-1);

	std::vector<vec3> V(N);
	for(unsigned int i = 0; i < m; i++){
		for(unsigned int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			unsigned int ij = i + j*m;
			V[ij] = bezier_surface(u, v, P);
		}
	}

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
	Elements<Lines> L{indices};

	int w = 1200, h = 800;
	ImageRGB G{w, h};
	mat4 View = lookAt({7, 7, 3}, {0, 0, 0}, {0, 0, 1});
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

		Render3D(V, L, shader, I);

		G.save_frame(k, "anim/output", "png");
	}
}
