#pragma once

#include <vector>
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"

struct Render2D_v1{
	RGB C;
	ImageRGB& image;
	
	template<class Vertex>
	void draw(Line<Vertex> line){
		vec2 L[] = { 
			toScreen(line[0]), 
			toScreen(line[1]) 
		};
			
		for(Pixel p: rasterizeLine(L))
			paint(p);
	}
	
	template<class Vertex>
	void draw(Triangle<Vertex> tri){
		vec2 T[] = { 
			toScreen(tri[0]), 
			toScreen(tri[1]), 
			toScreen(tri[2]) 
		};

		for(Pixel p: rasterizeTriangle(T))
			paint(p);
	}

	template<class Vertex>
	vec2 toScreen(const Vertex& P) const{
		vec2 p = get2DPosition(P);
		return {
			((p[0] + 1)*image.width() - 1)/2,
			((p[1] + 1)*image.height() - 1)/2
		};
	}

	void paint(Pixel p){
		if(p.x >= 0 && p.y >= 0 && p.x < image.width() && p.y < image.height())
			image(p.x,p.y) = C;
	}
};
	
template<class Prims, class Vertices>
void render2d(const Vertices& V, const Prims& p, RGB C, ImageRGB& image){
	Render2D_v1 R{C, image};
	for(auto primitive: assemble(p, V)) 
		R.draw(primitive);
}
