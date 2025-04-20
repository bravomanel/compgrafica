#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include <algorithm>
#include <cmath>
#include "vec.h"

//////////////////////////////////////////////////////////////////////////////

struct Pixel{
	int x, y;
};

inline Pixel toPixel(vec2 u){
	return { (int)round(u[0]), (int)round(u[1]) };
}

inline vec2 toVec2(Pixel p){
	return {(float)p.x, (float)p.y};
}

//////////////////////////////////////////////////////////////////////////////

template<class Line>
std::vector<Pixel> rasterizeLine(const Line& P){
	//return simple(P[0], P[1]);

	//return dda(P[0], P[1]);

	return bresenham(toPixel(P[0]), toPixel(P[1]));
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> simple(vec2 A, vec2 B){
	std::vector<Pixel> out;
	vec2 d = B - A;
	float m = d[1]/d[0];
	float b = A[1] - m*A[0];

	int x0 = (int)roundf(A[0]);
	int x1 = (int)roundf(B[0]);

	for(int x = x0; x <= x1; x++){
		int y = (int)roundf(m*x + b);
		out.push_back({x, y});
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> dda(vec2 A, vec2 B){
	vec2 dif = B - A;
	float delta = std::max(fabs(dif[0]), fabs(dif[1]));

	vec2 d = (1/delta)*dif;
	vec2 p = A;

	std::vector<Pixel> out;
	for(int i = 0; i <= delta; i++){
		out.push_back(toPixel(p));
		p = p + d;
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////

inline std::vector<Pixel> bresenham_base(Pixel p0, Pixel p1){
	std::vector<Pixel> out;
	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;
	int D = 2*dy - dx; 
	int y = p0.y;
	for(int x = p0.x; x <= p1.x; x++){
		out.push_back({x, y});
		if(D > 0){
			y++;
			D -= 2*dx;
		}
		D += 2*dy;
	}
	return out;
}

inline std::vector<Pixel> bresenham(Pixel p0, Pixel p1){
	if(p0.x > p1.x)
		std::swap(p0, p1);

	bool mirrorV = p1.y < p0.y;
	if(mirrorV)
		p1.y = 2*p0.y - p1.y;

	bool flip = (p1.x - p0.x) < (p1.y - p0.y);
	if(flip){
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
	}

	std::vector<Pixel> out = bresenham_base(p0, p1);

	if(flip){
		for(Pixel& p: out)
			std::swap(p.x, p.y);
		std::swap(p0.x, p0.y);
	}

	if(mirrorV){
		for(Pixel& p: out)
			p.y = 2*p0.y - p.y;
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////////

template<class Tri>
std::vector<Pixel> rasterizeTriangle(const Tri& P){
	return simple_rasterize_triangle(P);
	//return scanline(P);
}

template<class Tri>
std::vector<Pixel> simple_rasterize_triangle(const Tri& P){
	vec2 A = P[0];
	vec2 B = P[1];
	vec2 C = P[2];

	int xmin =  ceil(std::min({A[0], B[0], C[0]}));
	int xmax = floor(std::max({A[0], B[0], C[0]}));
	int ymin =  ceil(std::min({A[1], B[1], C[1]}));
	int ymax = floor(std::max({A[1], B[1], C[1]}));

	std::vector<Pixel> out;
	Pixel p;
	for(p.y = ymin; p.y <= ymax; p.y++)
		for(p.x = xmin; p.x <= xmax; p.x++)
			if(is_inside(toVec2(p), P))
				out.push_back(p);
	return out;
}


inline float intersection(vec2 P1, vec2 P2, int y){
    return 0; // TAREFA - AULA 06
}

template<class Tri>
std::vector<Pixel> scanline(const Tri& P){
	/* TAREFA - AULA 06*/
	std::vector<Pixel> out;
	return out;
}

#endif
