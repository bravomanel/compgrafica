#include "Primitives.h"
#include "rasterization.h"
#include "Image.h"

int main(){
	ImageRGB Img(500, 400);
	Img.fill(white);

	vec2 P[] = {
		{ 60, 105},
		{229, 114},
		{145, 270},
		{364, 208},
		{283, 333},
		{471, 298}
	};

	Triangles T{6};
	//TriangleStrip T{6};

	for(Triangle<vec2> triangle: assemble(T, P))
		for(Pixel p: rasterizeTriangle(triangle))
			Img(p.x,p.y) = blue;
	
	Img.savePNG("output.png");
}

