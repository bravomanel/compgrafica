#include "Render3D.h"
#include "SimpleShader.h"
#include "transforms.h"

int main(){
	vec3 V[] = {
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 1, 1},
	};
	unsigned int indices[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7
	};
	Elements<Lines> Le{indices};

	int w = 500, h = 400;

	mat4 Model = rotate_y(0.5)*rotate_x(0.1)*translate(-0.5, -0.5, -0.5);
	mat4 View = lookAt({0, 1, 2}, {0, 0, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 10);

	SimpleShader shader;
	shader.M = Projection*View*Model;
	shader.C = blue;

	ImageRGB G{w, h};
	G.fill(white);	
	Render3D(V, Le, shader, G);
	G.savePNG("output.png");
}
