#include "Render2D_v1.h"
#include "bezier.h"

int main(){
	vec2 P[] = {
		{-0.8,  0.2},
		{-0.2, -0.8},
		{-0.6,  0.8},
		{ 0.6,  0.9},
		{ 0.2,  0.4},
		{ 0.8,  0.0},
	};
	std::vector<vec2> Q = sample_bezier<5>(P, 100);

	LineStrip LP{6};
	LineStrip LQ{Q.size()};
	
	ImageRGB G(600, 600);
	G.fill(white);
	render2d(P, LP, red, G);
	render2d(Q, LQ, blue, G);
	G.save("output.png");
}
