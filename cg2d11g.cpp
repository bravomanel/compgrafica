#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"

int main(){
	std::vector<vec2> CP = loadCurve("curvaC.txt");
	std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);

	float t = 1.1;
	mat2 Rot = {
		cos(t), -sin(t),
		sin(t), cos(t)
	};
	mat2 Cis = {
		1, -0.5,
		0, 1
	};
	//std::vector<vec2> Q = (Cis*Rot)*P;
	std::vector<vec2> Q = (Rot*Cis)*P;

	LineStrip L{P.size()};
	
	ImageRGB G(800, 800);
	G.fill(white);
	render2d(P, L, red, G);
	render2d(Q, L, blue, G);
	G.save("output.png");
}
