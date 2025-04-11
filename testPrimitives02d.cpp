#include "Primitives.h"
#include "vec.h"
#include "rasterization.h"
#include "Image.h"

int main(){
	ImageRGB Img(500, 400);
	Img.fill(white);

	vec2 P[] = {
		{193, 66},
		{259, 139},
		{340, 54},		
		{176, 220},
		{336, 212},
		{261, 311}
	};

	unsigned int indices[] = {1, 0, 3, 5, 4, 2};

	Elements<TriangleFan> T{indices};

	for(Triangle<vec2> triangle: assemble(T, P))
		for(Pixel p: rasterizeTriangle(triangle))
			Img(p.x,p.y) = blue;
	
	Img.savePNG("output.png");
}

