#include "Render3D.h"
#include "SimpleShader.h"
#include "transforms.h"

int main(){
	std::vector<vec3> V = {
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 1, 1},
	};
	std::vector<unsigned int> indices = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7
	};
	Elements<Lines> L{indices};

	int w = 600, h = 600;
	mat4 Model = rotate_z(0.1);
	mat4 View = lookAt({1.2, 1.2, 1.2}, {0, 0, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(60, a, 0.4, 2);

	SimpleShader shader;
	shader.M = Projection*View*Model;
	shader.C = blue;
	
	ImageRGB G{w, h};
	G.fill(white);	
	Render3D(V, L, shader, G);
	G.savePNG("output.png");
}
