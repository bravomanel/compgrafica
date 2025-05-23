#include "Render3D.h"
#include "ZBuffer.h"
#include "ColorShader.h"
#include "VertexUtils.h"
#include "transforms.h"

int main(){
	unsigned int m = 30, n = 30, N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
              v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	std::vector<Vec3Col> V(N);
	for(unsigned int i = 0; i < m; i++){
		for(unsigned int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			unsigned int ij = i + j*m;
			V[ij].position = {u, v, sinf(u*v/4)};
			
			float s = i/(m-1.0);
			float t = j/(n-1.0);
			V[ij].color = bilerp(s, t, red, blue, green, yellow);
		}
	}

	std::vector<unsigned int> indices;
	for(unsigned int i = 0; i < m-1; i++){
		for(unsigned int j = 0; j < n-1; j++){	
			unsigned int ij = i + j*m;
			indices.insert(indices.end(), {
				ij, ij+1, ij+m,
				ij+m+1, ij+m, ij+1
			});
		}
	}
	Elements<Triangles> T{indices};

	int w = 600, h = 600;
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

		ColorShader shader;
		shader.M = Projection*View*Model;
		Render3D(V, T, shader, I);

		G.save_frame(k, "anim/output", "png");
	}
}
