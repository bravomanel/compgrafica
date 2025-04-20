#ifndef BEZIER_H
#define BEZIER_H

#include "vec.h"
#include <string.h>

template<size_t M, size_t N>
Vec<N> bezier(const Vec<N>* P, float t){
	Vec<N> Q[M+1];
	memcpy(Q, P, sizeof(Q));
	
	for(int m = M; m > 0; m--)
		for(int i = 0; i < m; i++)
			Q[i] = lerp(t, Q[i], Q[i+1]);
	
	return Q[0]; 
}

template<size_t M, size_t N>
std::vector<Vec<N>> sample_bezier(const Vec<N>* P, int n){
	std::vector<Vec<N>> C(n);
	for(int i = 0; i < n; i++){
		float t = i/(n-1.0);
		C[i] = bezier<M, N>(P, t);
	}
	return C;
}

template<size_t M, size_t N>
std::vector<Vec<N>> sample_bezier_spline(const std::vector<Vec<N>>& P, int n_samples){
	std::vector<Vec<N>> C;
	size_t n = P.size();
	for(size_t i = 0; i+M < n; i+=M){
		auto Q = sample_bezier<M>(P.data() + i, n_samples);
		C.insert(C.end(), Q.begin(), Q.end());
	}
	return C;
}

#endif
