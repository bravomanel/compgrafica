#include "Render2D.h"

struct Varying{
	vec2 position;
	vec3 color;
};


int main(){
#if 0
	clip_polygon = {
		Semiplane{ {-0.8, -0.4}, { 1,  0} }, 
		Semiplane{ { 0.8, -0.4}, {-1,  0} }, 
		Semiplane{ {-0.8, -0.4}, { 0,  1} }, 
		Semiplane{ { 0.8,  0.4}, { 0, -1} }
	};
#endif

	Varying P[] = {
		{{-0.9, -0.3}, toVec(red)},
		{{-0.6, -0.2}, toVec(green)},
		{{-0.8, -0.8}, toVec(blue)},
		{{ 0.2, -0.6}, toVec(red)},
		{{ 0.9,  0.5}, toVec(yellow)},
		{{ 0.0,  0.0}, toVec(cyan)},
		{{-0.6,  0.2}, toVec(magenta)},
		{{-0.4,  0.6}, toVec(brown)},
	};
	size_t n_verts = std::size(P);
	LineStrip L{n_verts};

	ImageRGB G(800, 800);
	G.fill(0xf5f5f5_rgb);

	auto vs = [&](int i){ return P[i]; };
	auto fs = [&](Varying v, RGB& color){
		color = toColor(v.color);
	};

	render2d(n_verts, L, vs, fs, G);
	
	G.savePNG("output.png");
}
