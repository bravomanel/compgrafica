#ifndef CLIP2D_H
#define CLIP2D_H

#include "vec.h"
#include "Primitives.h"


struct Semiplane{
	vec2 A;
	vec2 n;
	bool has(vec2 P) const{
		return dot(P-A, n) >= 0;
	}
	float intersect(vec2 P, vec2 Q) const{
		return dot(A-P, n)/dot(Q-P,n);
	}
};

static std::vector<Semiplane> clip_polygon = {
	Semiplane{ {-1, -1}, { 1,  0} }, 
	Semiplane{ { 1, -1}, {-1,  0} }, 
	Semiplane{ {-1, -1}, { 0,  1} }, 
	Semiplane{ { 1,  1}, { 0, -1} }
};

template<class Vertex>
bool line_clip(Line<Vertex>& line){
	float maxIn = 0;
	float minOut = 1;
	vec2 A = get2DPosition(line[0]);
	vec2 B = get2DPosition(line[1]);
	for(int i = 0; i < clip_polygon.size(); i++){
		if(!clip_polygon[i].has(A) && !clip_polygon[i].has(B)){
			return false;
		}
		else if(clip_polygon[i].has(A) && clip_polygon[i].has(B)){
			continue;
		}
		float t = clip_polygon[i].intersect(A, B);
		if(clip_polygon[i].has(A) && !clip_polygon[i].has(B)){
			minOut = std::min(minOut, t);
		}
		if(!clip_polygon[i].has(A) && clip_polygon[i].has(B)){
			maxIn = std::max(maxIn, t);
		}
		if(maxIn>minOut){
			return false;
		}
	}
	Line<Vertex> line_aux;
	line_aux[0] = mix_line(maxIn, line);
	line_aux[1] = mix_line(minOut, line);
	line = line_aux;
	return true;
}

template<class Vertex>
std::vector<Line<Vertex>> clip(const std::vector<Line<Vertex>>& lines){
	std::vector<Line<Vertex>> res;
	for(Line<Vertex> line: lines)
		if(line_clip(line))
			res.push_back(line);
	return res;
}


template<class Vertex>
std::vector<Vertex> poly_clip(const std::vector<Vertex>& polygon, Semiplane S){
	std::vector<Vertex> R;

	for(unsigned int i = 0; i < polygon.size(); i++){
		Line<Vertex> L = { polygon[i], polygon[(i+1)%polygon.size()] };

		vec2 p = get2DPosition(L[0]);
		vec2 q = get2DPosition(L[1]);
	
		bool Pin = S.has(p);
		bool Qin = S.has(q);

		if(Pin != Qin)
			R.push_back( mix_line(S.intersect(p, q), L) );
		
		if(Qin)
			R.push_back(L[1]);
	}
	return R;
}

template<class Vertex>
std::vector<Vertex> poly_clip(const std::vector<Vertex>& polygon){
	// Algoritmo de Sutherland-Hodgeman
   	std::vector<Vertex> R = polygon;

 	for(Semiplane S: clip_polygon)
		R = poly_clip(R, S);

   return R;
}

template<class Vertex>
std::vector<Triangle<Vertex>> clip(const std::vector<Triangle<Vertex>>& tris){
	std::vector<Triangle<Vertex>> res;

	for(Triangle<Vertex> tri: tris){
		std::vector<Vertex> polygon = {tri[0], tri[1], tri[2]};
		polygon = poly_clip(polygon);

		TriangleFan T{polygon.size()};
		for(Triangle<Vertex> tri: assemble(T, polygon))
			res.push_back(tri);
	}

	return res;
}

#endif
