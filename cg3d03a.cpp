#include "Render3D.h"
#include "SimpleShader.h"
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
	TriLines<Elements<Triangles>> L{indices};

	int w = 600, h = 600;
	mat4 Model = rotate_z(0.1);
	mat4 View = lookAt({1.5, 1.5, 1.5}, {0, 0, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(60, a, 0.1, 5);

	ColorShader shader;
	shader.M = Projection*View*Model;
	
	SimpleShader simpleshader;
	simpleshader.M = Projection*View*Model;
	simpleshader.C = black;

	ImageRGB G{w, h};
	G.fill(white);	
	Render3D(V, T, shader, G);
//	Render3D(V, L, simpleshader, G);
	G.savePNG("output.png");
}
