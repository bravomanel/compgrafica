#pragma once
#include "vec.h"
#include <math.h>

template<size_t N>
struct Dual{
	float f;
	Vec<N> df;

	friend Dual operator+(float C, Dual A){ return {C+A.f, A.df}; }

	friend Dual operator+(Dual A, Dual B){ return {A.f + B.f,  A.df + B.df}; }
	
    friend Dual operator+(Dual A, float C){ return {A.f + C, A.df}; }
	
	friend Dual operator-(Dual A, Dual B){	return {A.f - B.f, A.df - B.df}; }

	friend Dual operator-(Dual A, float C){ return {A.f - C, A.df}; }

	friend Dual operator-(float C, Dual A){ return {C-A.f, -A.df}; }

	friend Dual operator*(float a, Dual A){ return {a*A.f, a*A.df}; }

	friend Dual operator*(Dual A, Dual B){ return {A.f*B.f, B.f*A.df + A.f*B.df}; }

	friend Dual operator/(Dual A, float a){ return {A.f/a, (1/a)*A.df }; }

	friend Dual operator/(Dual A, Dual B){ return {A.f/B.f, (B.f*A.df - A.f*B.df)/(B.df*B.df) }; }

	friend Dual pow(Dual A, float n){ return {pow(A.f, n), n*pow(A.f,n-1)*A.df}; }

	friend Dual sin(Dual A){ return { sin(A.f), cos(A.f)*A.df }; }

	friend Dual cos(Dual A){ return { cos(A.f), -sin(A.f)*A.df }; }

	friend Dual sqrt(Dual A){ return pow(A, 0.5);  }

};