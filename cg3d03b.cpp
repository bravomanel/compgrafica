#include "Render3D.h"
#include "ColorShader.h"
#include "VertexUtils.h"
#include "transforms.h"
#include "ZBuffer.h"

int main(){
	Vec3Col V[] = {
		{{0, 0, 0}, red},
		{{1, 0, 0}, red},
		{{1, 1, 0}, green},
		{{0, 1, 0}, green},
		{{0, 0, 1}, blue},
		{{1, 0, 1}, blue},
		{{1, 1, 1}, yellow},
		{{0, 1, 1}, yellow}
	};

	unsigned int indices[] = {
        0, 1, 5,   0, 5, 4,
        0, 3, 7,   0, 7, 4,
        0, 1, 2,   0, 2, 3,
		4, 5, 7,   6, 7, 5,
		5, 1, 6,   2, 6, 1,
		6, 2, 7,   3, 7, 2,
	};
	Elements<Triangles> T{indices};

	int w = 500, h = 500;

	mat4 View = translate(0, 0, -3);
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 10);

	ImageRGB G{w, h};

	int nframes = 60;
	for(int k = 0; k < nframes; k++){
		G.fill(white);
		ImageZBuffer IZ{G};
			
		float theta = k*2*M_PI/nframes;
	
		mat4 Model = rotate_y(theta)*rotate_x(0.1)*
		              translate(-0.5, -0.5, -0.5);
	
		ColorShader shader;
		shader.M = Projection*View*Model;
		Render3D(V, T, shader, G);
	
		G.save_frame(k, "anim/output", "png");
	}
}
