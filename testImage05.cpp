#include "Image.h"

float G(float x, float y, float cx, float cy, float s){
	float dx = x - cx;
	float dy = y - cy;
	return 100*exp(-(dx*dx + dy*dy)/(s*s));
}

float F(float x, float y){
	return 50 
		- G(x, y, 200, 300, 50) 
		- G(x, y, 350, 450, 100) 
		- G(x, y, 350, 220, 120);
}

int main(){
	ImageRGB img(600, 600);
	img.fill(white);
	for(int y = 0; y < img.height(); y++)
		for(int x = 0; x < img.width(); x++)
			if(F(x,y) <= 0)
				img(x,y) = red;
		

	img.save("output.png");
}
