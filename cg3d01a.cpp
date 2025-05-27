#include "Render3D.h"
#include "Image.h"
#include "Primitives.h"
#include "SimpleShader.h"
#include "transforms.h"

int main(){
	vec3 V[] = {
		{-1, -1, -1},
		{1, -1, -1},
		{1, 1, -1},
		{-1, 1, -1},
		{-1, -1, 1},
		{1, -1, 1},
		{1, 1, 1},
		{-1, 1, 1},
	};
	unsigned int indices[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7
	};
	Elements<Lines> Le{indices};
	
	mat4 Model = rotate_y(0.2)*rotate_x(0.1);
	mat4 View = lookAt({0, 0, 4}, {0, 0, 0}, {0, 1, 0});
	mat4 Projection = orthogonal(-2, 2, -2, 2, 1, 10);
	//mat4 Projection = perspective(60, 1, 0.1, 100);

	SimpleShader shader;
	shader.M = Projection*View*Model;
	shader.C = blue;

	ImageRGB image{500, 500};
	image.fill(white);
	Render3D(V, Le, shader, image);
	image.savePNG("output.png");
}
