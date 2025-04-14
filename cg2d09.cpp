#include "Render2D_v1.h"
#include "bezier.h"

int main(){
	std::vector<vec2> P = {
		{-0.8,  0.0},
		{-0.6, -0.6},
		{ 0.6, -0.8},
		{ 0.8,  0.3},
		{ 0.4,  0.8}, 
		{-0.3,  0.4},
		{ 0.2,  0.3}
	};
	std::vector<vec2> Q = sample_bezier_spline<4>(P, 50);
	
	LineStrip LP{P.size()};
	LineStrip LQ{Q.size()};

	ImageRGB G(600, 600);
	G.fill(white);
	render2d(P, LP, blue, G);
	render2d(Q, LQ, red, G);
	G.save("output.png");
}
