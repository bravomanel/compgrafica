#pragma once

#include <vector>
#include "Image.h"
#include "Primitives.h"
#include "rasterization.h"
#include "Clip2D.h"

template<class Varying, class FShader>
struct Render2D{
	FShader& fragment_shader;
	ImageRGB& image;

	void draw(Line<Varying> line){
		vec2 L[] = { 
			toScreen(line[0]), 
			toScreen(line[1]) 
		};
			
		for(Pixel p: rasterizeLine(L)){
			float t = find_mix_param(toVec2(p), L[0], L[1]);
			Varying vi = mix_line(t, line);
			paint(p, vi);
		}
	}
	
	void draw(Triangle<Varying> tri){
		/* TAREFA - AULA 09 */
		vec2 T[] = { 
			toScreen(tri[0]), 
			toScreen(tri[1]), 
			toScreen(tri[2]) 
		};

		for(Pixel p: rasterizeTriangle(T)){
			vec3 t = barycentric_coords(toVec2(p), T);
			Varying vi = mix_triangle(t, tri);
			paint(p, vi);
		}
	}

	vec2 toScreen(Varying P) const{
		vec2 p = get2DPosition(P);
		return {
			((p[0] + 1)*image.width() - 1)/2,
			((p[1] + 1)*image.height() - 1)/2
		};
	}

	void paint(Pixel p, Varying v){
		if(p.x >= 0 && p.y >= 0 && p.x < image.width() && p.y < image.height())
			fragment_shader(v, image(p.x,p.y));
	}
};

template<class Prims, class VShader, class FShader>
void render2d(size_t N, const Prims& p, VShader vertex_shader, FShader fragment_shader, ImageRGB& image){
	using Varying = decltype(vertex_shader(0));

	// Pipeline de renderização
	// Transformação de vértices
	std::vector<Varying> V(N); 
	for(size_t i = 0; i < N; i++)
		V[i] = vertex_shader(i);

	Render2D<Varying, FShader> R{fragment_shader, image};

	// Montagem de primitivas, Recorte 2D
	for(auto primitive: clip(assemble(p, V))) 
		R.draw(primitive); // rasterização
}

template<class Prims, class Vertices>
void render2d(const Vertices& V, const Prims& p, RGB C, ImageRGB& image){
	using Varying = decltype(V[0]);
	auto vs = [&](int i){ return V[i]; };
	auto fs = [&](Varying, RGB& color){ color = C; };
	render2d(std::size(V), p, vs, fs, image);
}