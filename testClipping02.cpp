#include "Render2D.h"

struct Varying{
	vec2 position;
	vec3 color;
};


int main(){
#if 0
	clip_polygon = {
		Semiplane{ {-0.6, -0.4}, { 1,  0} }, 
		Semiplane{ { 0.6, -0.4}, {-1,  0} }, 
		Semiplane{ {-0.6, -0.4}, { 0,  1} }, 
		Semiplane{ { 0.6,  0.4}, { 0, -1} }
	};
#elif 0
	clip_polygon = {
		Semiplane{ {-0.3, -0.1}, { 0.16,  0.15} }, 
		Semiplane{ { 0.6, 0.2}, {-0.22,  0.23} }, 
		Semiplane{ { 0, 0.5}, { 0,  -1} }, 
	};
#endif

	Varying P[] = {
		{{-0.4, -0.6}, toVec(red)},
		{{-0.2, -0.8}, toVec(green)},
		{{-0.8, -0.8}, toVec(blue)},
		
		{{-0.2, 0}, toVec(red)},
		{{0.2, -0.6}, toVec(yellow)},
		{{-0.8, 0}, toVec(cyan)},
		
		{{0, 0}, toVec(magenta)},
		{{0.3, -0.2}, toVec(brown)},
		{{0.4, -0.6}, toVec(blue)},
		
		{{0.3, 0}, toVec(green)},
		{{0.7, 0.8}, toVec(yellow)},
		{{0.8, -0.8}, toVec(cyan)},
		
		{{0, 0.2}, toVec(orange)},
		{{0.2, 0.8}, toVec(green)},
		{{0.4, 0.6}, toVec(gray)},
		
		{{-0.4, 0.2}, toVec(red)},
		{{-0.2, 0.6}, toVec(brown)},
		{{-0.8, 0.1}, toVec(blue)},
	};
	size_t n_verts = std::size(P);
	Triangles L{n_verts};

	ImageRGB G(800, 800);
	G.fill(0xf5f5f5_rgb);

	auto vs = [&](int i){ return P[i]; };
	auto fs = [&](Varying v, RGB& color){
		color = toColor(v.color);
	};

	render2d(n_verts, L, vs, fs, G);
	
	G.savePNG("output.png");
}
