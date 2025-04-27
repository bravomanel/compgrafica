#include "Render2D_v1.h"
#include "bezier.h"
#include "polygon_triangulation.h"

int main(){
	std::vector<vec2> CP = loadCurve("tarefa07escudo.txt");
	std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);
	std::vector<unsigned int> indices = triangulate_polygon(P);

	LineStrip L{P.size()};
	Elements<Triangles> T{indices};

	ImageRGB G(800, 800);
	G.fill(white);
	render2d(P, T, blue, G);
	G.save("output.png");
}
