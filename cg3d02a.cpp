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
		4, 5, 7,   6, 7, 5,
		5, 1, 6,   2, 6, 1,
		6, 2, 7,   3, 7, 2,
        0, 1, 5,   0, 5, 4,
        0, 3, 7,   0, 7, 4,
        0, 1, 2,   0, 2, 3

	};

	Elements<Triangles> T{indices};
	
	mat4 Model = rotate_y(-0.2)*rotate_x(0.1)*translate(-0.5, -0.5, -0.5);
	mat4 View = lookAt({0, 0, 2}, {0, 0, 0}, {0, 1, 0});
	mat4 Projection = orthogonal(-1, 1, -1, 1, 0.1, 10);

	SimpleShader shader;
	shader.M = Projection*View*Model;
	shader.C = blue;

	ImageRGB G{500, 500};
	G.fill(white);	
	Render3D(V, T, shader, G);
	G.savePNG("output.png");
}
