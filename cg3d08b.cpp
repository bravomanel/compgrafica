#include "Render3D.h"
#include "ZBuffer.h"
#include "bezier.h"
#include "ColorShader.h"
#include "VertexUtils.h"
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
		{-3, -1, 5}, {-1, -1, 2}, {1, -1, 0}, {3, -1, 1},
		{-3,  1,-1}, {-1,  1, 1}, {1,  1, 0}, {3,  1, 0},
		{-3,  3, 0}, {-1,  3,-1}, {1,  3, 1}, {3,  3, 1},
	};
	unsigned int m = 20, n = 20, N = m*n;

	float u0 = 0, u1 = 1, du = (u1-u0)/(m-1),
              v0 = 0, v1 = 1, dv = (v1-v0)/(n-1);

	std::vector<Vec3Col> V(N);
	for(unsigned int i = 0; i < m; i++){
		for(unsigned int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			int ij = i + j*m;
			V[ij].position = bezier_surface(u, v, P);
			
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
	mat4 View = lookAt({7, 7, 3}, {0, 0, 0}, {0, 0, 1});
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
