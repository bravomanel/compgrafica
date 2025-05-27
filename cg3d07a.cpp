#include "Render3D.h"
#include "ZBuffer.h"
#include "SimpleShader.h"
#include "transforms.h"

int main(){
	int m = 50, n = 50, N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
              v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	std::vector<vec3> P(N);
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			int ij = i + j*m;
			P[ij] = {u, v, sinf(u*v/4)};
		}
	}

	std::vector<unsigned int> indices;
	for(int i = 0; i < m-1; i++){
		for(int j = 0; j < n-1; j++){	
			unsigned int ij = i + j*m;
			indices.insert(indices.end(), {
				ij, ij+1, ij+m,
				ij+m+1, ij+m, ij+1
			});
		}
	}
	TriLines<Elements<Triangles>> L{indices};

	SimpleShader shader;
	shader.C = blue;

	int w = 800, h = 600;
	ImageRGB G{w, h};
	mat4 View = lookAt({10,10,10}, {0, 0, 0}, {0, 0, 1});
	float a = w/(float)h;
	mat4 Projection = perspective(50, a, 0.1, 50);

	int nframes = 30;
	for(int k = 0; k < nframes; ++k){
		G.fill(white);
		ImageZBuffer I{G};

		float theta = k*2*M_PI/nframes;
		mat4 Model = rotate_z(theta);
		
		shader.M = Projection*View*Model;
		Render3D(P, L, shader, I);

		G.save_frame(k, "anim/output", "png");
	}
}
