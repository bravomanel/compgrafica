#include "Render2D_v1.h"
#include "matrix.h"
#include "bezier.h"

int main(){
	std::vector<vec2> CP = loadCurve("curvaC.txt");
	std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);

	float t = 1.1;
	mat2 M = {
		cos(t), -sin(t),
		sin(t), cos(t)
	};

	LineStrip L{P.size()};
	
	ImageRGB G(800, 800);
	G.fill(white);
	render2d(P, L, red, G);
	render2d(M*P, L, blue, G);
	G.save("output.png");
}
