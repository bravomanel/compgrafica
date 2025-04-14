#include "Render2D_v1.h"
#include "bezier.h"

int main(){
	vec2 P[] = {
		{-0.4,  0.8},
		{-0.8, -0.6},
		{ 0.8, -0.8},
		{ 0.4,  0.2}
	};
	LineStrip LP{4};

	std::vector<vec2> Q = sample_bezier<3>(P, 50);
	LineStrip LQ{Q.size()};
	
	ImageRGB G(600, 600);
	G.fill(white);
	render2d(P, LP, red, G);
	render2d(Q, LQ, blue, G);
	G.save("output.png");
}
