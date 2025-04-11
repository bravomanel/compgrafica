#include "Primitives.h"
#include "rasterization.h"
#include "Image.h"

int main(){
	vec2 P[] = {
		{20, 20},
		{80, 60},
		{220, 30},
		{350, 90},
		{70, 300},
		{320, 150}
	};
	
	ImageRGB Img(400, 400);
	Img.fill(white);

	Lines L{6};
	//LineStrip L{6};
	//LineLoop L{6};
	
	for(Line<vec2> line: assemble(L, P))
		for(Pixel p : rasterizeLine(line))
			Img(p.x, p.y) = red;
	
	Img.savePNG("output.png");
}

