#include "Render3D.h"
#include "ZBuffer.h"
#include "ColorShader.h"
#include "VertexUtils.h"
#include "transforms.h"

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

	int w = 600, h = 600;
	mat4 Model = rotate_y(3.2)*translate(-0.5, -0.5, -0.5);
	mat4 View = lookAt({1.1, 0.8, 1.1}, {0, 0, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(60, a, 0.1, 5);

	ColorShader shader;
	shader.M = Projection*View*Model;

	ImageRGB G{w, h};
	ImageZBuffer IZ{G}; 
	G.fill(white);	
	//Render3D(V, T, shader, G);
	Render3D(V, T, shader, IZ);
	G.savePNG("output.png");
}
