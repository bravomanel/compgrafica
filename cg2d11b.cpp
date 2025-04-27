#include "Render2D_v1.h"
#include "bezier.h"

int main(){
	std::vector<vec2> CP = loadCurve("curvaC.txt");
	std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);

	vec2 v = {-1.0, -0.5};

	std::vector<vec2> Q(P.size());
	for(unsigned int i = 0; i < P.size(); i++)
		Q[i] = P[i] + v;
	
	LineStrip L{P.size()};
	
	ImageRGB G(800, 800);
	G.fill(white);
	render2d(P, L, red, G);
	render2d(Q, L, blue, G);
	G.save("output.png");
}
