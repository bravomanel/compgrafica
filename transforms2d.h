#pragma once

#include "matrix.h"
#include "vec.h"

inline mat3 translate(vec2 u){
	return {
		1, 0, u[0],
		0, 1, u[1],
		0, 0,    1
	};
}

inline mat3 translate(float x, float y){
    return translate(vec2{x, y});
}

inline mat3 scale(float x, float y){
	return {
		x, 0, 0,
		0, y, 0,
		0, 0, 1
	};
}

inline mat3 skew(float a){
   return {
       1, a, 0,
       0, 1, 0,
       0, 0, 1
   };
}

inline mat3 rotate_2d(float angle){
	float c = cos(angle);
	float s = sin(angle);

	return {
		c, -s, 0, 
		s,  c, 0, 
		0,  0, 1,
	};
}

inline mat3 window(float l, float r, float b, float t){
    return {
        2/(r-l),        0,   (l+r)/(l-r),
              0,  2/(t-b),   (b+t)/(b-t),
              0,        0,             1
    };
}