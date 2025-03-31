#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include "vec.h"

using Byte = unsigned char;

template<size_t N>
using Color = std::array<Byte, N>;

using RGB  = Color<3>;
using RGBA = Color<4>;

inline RGB operator""_rgb(unsigned long long u){
	return RGB{
		(Byte)(u >> 16),
		(Byte)(u >>  8),
		(Byte)u
	};
}

inline RGBA operator""_rgba(unsigned long long u){
	return RGBA{
		(Byte)(u >> 24),
		(Byte)(u >> 16),
		(Byte)(u >>  8),
		(Byte)u
	};
}


const RGB white   = 0xffffff_rgb;
const RGB red     = 0xff0000_rgb;
const RGB green   = 0x00ff00_rgb;
const RGB blue    = 0x0000ff_rgb;
const RGB black   = 0x000000_rgb;
const RGB cyan    = 0x00ffff_rgb;
const RGB yellow  = 0xffff00_rgb;
const RGB magenta = 0xff00ff_rgb;
const RGB gray    = 0x808080_rgb;
const RGB orange  = 0xffa500_rgb;
const RGB purple  = 0x800080_rgb;
const RGB brown   = 0xa08060_rgb;

// Retorna o valor mais próximo de v no intervalo [a,b].
inline float clamp(float v, float a, float b){
	if(v < a)
		return a;
	else if(v > b)
		return b;
	return v;
}

// Converte de Byte para float.
inline float toFloat(Byte v){
	return v/255.0f;
}

// Converte de float para Byte.
inline Byte toByte(float v){
	return (Byte)roundf(255*clamp(v, 0.0f, 1.0f));
}

// Converte de Color<N> para Vec<N>.
template<size_t N>
Vec<N> toVec(const Color<N>& C){
	Vec<N> V;
	for(size_t i = 0; i < N; i++)
		V[i] = toFloat(C[i]);
	return V;
}

// Converte de Vec<N> para Color<N>.
template<size_t N>
Color<N> toColor(const Vec<N>& V){
	Color<N> C;
	for(size_t i = 0; i < N; i++)
		C[i] = toByte(V[i]);
	return C;
}

// Interpolacao linear de duas cores A e B
template<size_t N>
Color<N> lerp(float t, Color<N> A, Color<N> B){
	return toColor(lerp(t, toVec(A), toVec(B)));
}

// Interpolacao bilinear de duas cores A e B
template<size_t N>
Color<N> bilerp(float u, float v, Color<N> A, Color<N> B, Color<N> C, Color<N> D){
	Vec<N> vA = toVec(A);
	Vec<N> vB = toVec(B);
	Vec<N> vC = toVec(C);
	Vec<N> vD = toVec(D);
	return toColor(bilerp(u, v, vA, vB, vC, vD));
}

#endif
