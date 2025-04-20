#include "Image.h"
#include "rasterization.h"
	
int main(){
	ImageRGB Img(600, 600);
	Img.fill(white);

	vec2 P[] = {
		{100, 300}, {200, 300},  {100, 200},
		{300, 200}, {400, 200},	{400, 300},
		{200, 400},{300, 300},{300, 450},
		{199.5,6.30645},{145,43},{199.5,31.5263},
		{150, 250}, {250, 200}, {350, 50}
	};

	auto draw = [&](int i, int j, int k, RGB c){
		vec2 T[3] = {P[i], P[j], P[k]};
		for(Pixel p : rasterizeTriangle(T))
			Img(p.x, p.y) = c;
	};

	draw(0, 1, 2, green);
	draw(3, 4, 5, green);
	draw(6, 7, 8, green);
	draw(9, 10, 11, green);
	draw(12, 13, 14, green);

	draw(1, 1, 1, red);
	draw(3, 3, 4, red);
	draw(6, 8, 8, red);

	Img.savePNG("output.png");
}