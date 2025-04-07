#include "Image.h"

float G(float x, float y, float cx, float cy, float s){
	float dx = x - cx;
	float dy = y - cy;
	return 100*exp(-(dx*dx + dy*dy)/(s*s));
}

float F(float x, float y, float t){
	vec2 c0 = {100, 100};
	vec2 c1 = {400, 500};
	vec2 c = lerp(t, c0, c1);
	return 50 - G(x, y, c[0], c[1], 50) - G(x, y, 350, 220, 120);
}

int main(){
	ImageRGB img(600, 600);

	int N = 60;
	for(int i = 0; i < N; i++){
		float t = i/(N-1.0);

		img.fill(white);
		for(int y = 0; y < img.height(); y++)
			for(int x = 0; x < img.width(); x++)
				if(F(x,y, t) <= 0)
					img(x,y) = red;

		img.save_frame(i, "anim/output", "png");
	}
}
