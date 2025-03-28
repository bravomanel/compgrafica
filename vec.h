#ifndef VEC_H
#define VEC_H

#include "matrix.h"

template<size_t M>
using Vec = Mat<M, 1>;

using vec2 = Vec<2>;
using vec3 = Vec<3>;
using vec4 = Vec<4>;

template<size_t M>
Vec<M> operator*(const Vec<M>& U, const Vec<M>& V){
    Vec<M> R;
    for(size_t i = 0; i < M; i++)
        R[i] = U[i]*V[i];
    return R;
}

template<size_t M>
Vec<M> operator/(const Vec<M>& U, const Vec<M>& V){
    Vec<M> R;
    for(size_t i = 0; i < M; i++)
        R[i] = U[i]/V[i];
    return R;
}

template<size_t M>
float dot(const Vec<M>& U, const Vec<M>& V){
	return dot<M, Vec<M>, Vec<M>>(U, V);
}

template<size_t M>
Vec<M> normalize(const Vec<M>& u){
	return (1.0/norm(u))*u;
}

inline vec3 reflect(vec3 I, vec3 N){
    return I - 2*dot(N, I)*N;
}

template<size_t M, size_t N>
std::vector<Vec<M>> operator*(const Mat<M, N>& A, const std::vector<Vec<N>>& P){
	int n = P.size();
	std::vector<Vec<M>> Q(n);
	for(int i = 0; i < n; i++)
		Q[i] = A*P[i];
	return Q;
}

inline vec2 toVec2(vec2 u){
    return u;
}

inline vec2 toVec2(vec3 u){
    return {u[0], u[1]};
}

inline vec2 toVec2(vec4 u){
    return {u[0], u[1]};
}

inline vec3 toVec3(vec2 u, float z=0){
    return {u[0], u[1], z};
}

inline vec3 toVec3(vec3 u){
    return u;
}

inline vec3 toVec3(vec4 u){
    return {u[0], u[1], u[2]};
}

inline vec4 toVec4(vec2 u, float z=0, float w=1){
    return {u[0], u[1], z, w};
}

inline vec4 toVec4(vec3 u, float w=1){
    return {u[0], u[1], u[2], w};
}

inline vec4 toVec4(vec4 u){
    return u;
}

inline std::vector<vec2> loadCurve(std::string filename){
	std::ifstream in(filename);
	int n = 0;
	in >> n;
	std::vector<vec2> P(n);
	for(vec2& v: P)
		in >> v[0] >> v[1];
	return P;
}

template<size_t M>
Vec<M> lerp(float t, Vec<M> A, Vec<M> B){
	//return (1-t)*A + t*B;
	return A + t*(B-A);
}

template<size_t M>
Vec<M> bilerp(float s, float t, Vec<M> A, Vec<M> B, Vec<M> C, Vec<M> D){
	Vec<M> E = lerp(s, A, B);
	Vec<M> F = lerp(s, C, D);
	return lerp(t, E, F);
}

inline float find_mix_param(vec2 v, vec2 v0, vec2 v1){
	vec2 d = v1 - v0;
	return dot(d, v-v0)/dot(d,d);
}

inline vec3 cross(vec3 u, vec3 v){
	return {
		 u[1]*v[2] - u[2]*v[1],
		 u[2]*v[0] - u[0]*v[2],
		 u[0]*v[1] - u[1]*v[0]
	};
}

template<size_t M, class...V, size_t N = 1+sizeof...(V)>
Mat<M, N> toMat(const Vec<M>& C, V... tail){
	std::array<Vec<M>, N> columns = {C, tail...};
	Mat<M, N> R;
	for(size_t j = 0; j < N; j++)
		R.column(j) = columns[j];
    return R;
}

template<class T, int N = sizeof(T)/sizeof(float)>
Vec<N>& asVec(T& t){
    return reinterpret_cast<Vec<N>&>(t);
}

inline vec2 get2DPosition(vec2 v){
	return v;
}

inline vec2 get2DPosition(vec3 v){
	return {v[0]/v[2], v[1]/v[2]};
}

inline vec2 get2DPosition(vec4 v){
	return {v[0]/v[3], v[1]/v[3]};
}

template<class Vertex>
inline vec2 get2DPosition(const Vertex& v){
	return get2DPosition(v.position);
}

inline vec3 operator*(const mat3& A, vec2 P){
	return A*toVec3(P, 1);
}

inline std::vector<vec3> operator*(const mat3& A, const std::vector<vec2>& Ps){
	std::vector<vec3> Q;

	for(vec2 P: Ps)
		Q.push_back( A*P );

	return Q;
}
/*****************************************************************************/

inline vec4 getPosition(vec2 v){
	return {v[0], v[1], 0, 1};
}

inline vec4 getPosition(vec3 v){
	return {v[0], v[1], v[2], 1};
}

inline vec4 getPosition(vec4 v){
	return v;
}

template<class Vertex>
inline vec4 getPosition(const Vertex& v){
	return getPosition(v.position);
}


template<class Line>
auto mix_line(float t, Line& L){
	auto v = L[0];
	asVec(v) = lerp(t, asVec(L[0]), asVec(L[1]));
	return v;
}

template<class Tri>
auto mix_triangle(vec3 a, Tri& T){
	auto v = T[0];
	asVec(v) = a[0]*asVec(T[0]) + a[1]*asVec(T[1]) + a[2]*asVec(T[2]);
	return v;
}

inline float tri_area(vec2 a, vec2 b, vec2 c){
	return (c[0] - a[0]) * (b[1] - a[1]) / 2 - (b[0] - a[0]) * (c[1] - a[1]) / 2;
}

template<class Tri>
vec3 barycentric_coords(vec2 p, const Tri& P){
	float aT = tri_area(P[0], P[1], P[2]);
	return {
		tri_area(   p, P[1], P[2])/aT,
		tri_area(P[0],    p, P[2])/aT,
		tri_area(P[0], P[1],    p)/aT
	};
}

template<class Tri>
bool is_inside(vec2 v, const Tri& P){
	float aT = tri_area(P[0], P[1], P[2]);
	if(fabs(aT) < 1e-15){
		float M[] = {
			norm(P[0]-P[1]),
			norm(P[1]-P[2]),
			norm(P[2]-P[0])
		};

		float maxM = M[0];
		int i = 0, j = 1;

		if(M[1] > maxM){
			i = 1;
			j = 2;
			maxM = M[1];
		}

		if(M[2] > maxM){
			i = 2;
			j = 0;
			maxM = M[2];
		}

		float a = tri_area(v, P[i], P[j]);
		return fabs(a) < 1e-15;
	}

	vec3 b = barycentric_coords(v, P);
	return b[0] >= 0 && b[1] >= 0 && b[2] >= 0;
}

#endif