#include "Render2D.h"
#include "matrix.h"
#include "transforms.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



int main(){
	// Coordenadas do modelo
	std::vector<vec4> V = {
		{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, 
		{-1, 1, -1, 1},{-1, -1, 1, 1}, {1, -1, 1, 1}, 
		{1, 1, 1, 1}, {-1, 1, 1, 1}, {0, 1.6, -1, 1}, {0, 1.6, 1, 1}
	};
	unsigned int indices[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7,
		9, 7,   9, 6,   3, 8,   8, 2,   9, 8
	};

	int w = 600, h = 600;
	// mat4 Projection = orthogonal(-2, 2, -2, 2, 4, 10);
	//mat4 Projection = frustum(-2, 2, -2, 2, 4, 10);
	mat4 Projection = perspective(60, w/(float)h, 4, 10);
	mat4 View = lookAt({3, 2, 4}, {0, 0, 0}, {0, 1, 0});
	mat4 Model = loadIdentity();

	mat4 M = Projection*View*Model;

	Elements<Lines> L{indices};
	
	ImageRGB G(w, h);
	G.fill(white);
	render2d(M*V, L, red, G);
	G.savePNG("output.png");
}
