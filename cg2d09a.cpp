#include "Render2D_v1.h"
#include "bezier.h"
#include "transforms2d.h"

int main(){
	std::vector<vec3> P = {
		{1, 0, 1},
		{1, 1, 1},
		{0, 2, 2},
        /*
		{-1, 1, 1},
		{-1, 0, 1},
		{-1, -1, 1},
        {0, -2, 2},
		{1, -1, 1},
		{1, 0, 1},
        */
		
	};

	std::vector<vec3> A = sample_bezier_spline<2>(P, 50);
    mat3 W = window(-1.1, 1.1, -1.1, 1.1);

	LineStrip L{A.size()};

	ImageRGB G(600, 600);
	G.fill(white);
	render2d(W*A, L, blue, G);
	G.save("output.png");
}
