#include "Render2D_v1.h"
#include "bezier.h"

int main(){
	std::vector<vec2> CP = loadCurve("tarefa07escudo.txt");
	std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);

	LineStrip L{P.size()};

	ImageRGB G(800, 800);
	G.fill(white);
	render2d(P, L, blue, G);
	G.save("output.png");
}
