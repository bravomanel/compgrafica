#include "Render3D.h"
#include "ZBuffer.h"
#include "SimpleShader.h"
#include "transforms.h"

int main(){
	unsigned int N = 150;
	float t0 = 0, t1 = 2*M_PI;
	float dt = (t1-t0)/(N-1);

	std::vector<vec3> V(N);
	for(unsigned int i = 0; i < N; i++){
		float t = t0 + i*dt;
		V[i] = {cosf(3*t), sinf(2*t), cosf(4*t)};
	}
	LineStrip L{N};
	
	SimpleShader shader;
	shader.C = blue;

	int w = 600, h = 600;
	ImageRGB G{w, h};
	mat4 View = lookAt({4,0,1}, {0, 0, 0}, {0, 0, 1});
	float a = w/(float)h;
	mat4 Projection = perspective(50, a, 0.1, 50);

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
