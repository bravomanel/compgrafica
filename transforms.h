#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "matrix.h"
#include "vec.h"
#include "stb_image_write.h"

inline mat4 loadIdentity(){
    return Id<4>();
}

inline mat4 translate(vec3 u){
	return {
		1, 0, 0, u[0],
		0, 1, 0, u[1],
		0, 0, 1, u[2],
		0, 0, 0,    1
	};
}

inline mat4 translate(float x, float y, float z){
    return translate(vec3{x, y, z});
}

inline mat4 scale(float x, float y, float z){
	return {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
}

inline mat4 rotate_x(float angle){
	float c = cos(angle);
	float s = sin(angle);

	return {
		1, 0,  0, 0,
		0, c, -s, 0,
		0, s,  c, 0,
		0, 0,  0, 1
	};
}

inline mat4 rotate_y(float angle){
	float c = cos(angle);
	float s = sin(angle);

	return {
		 c, 0, s, 0,
		 0, 1, 0, 0,
		-s, 0, c, 0,
		 0, 0, 0, 1
	};
}

inline mat4 rotate_z(float angle){
	float c = cos(angle);
	float s = sin(angle);

	return {
		c, -s, 0, 0,
		s,  c, 0, 0,
		0,  0, 1, 0,
		0,  0, 0, 1
	};
}

inline mat4 rotate(vec3 n, float theta){
    // Normaliza o vetor do eixo de rota  o
    n = normalize(n);

    // Calcula os componentes trigonom tricos
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    float oneMinusCos = 1.0f - cosTheta;

    float nx = n[0], ny = n[1], nz = n[2];

    // Cria  o manual da matriz de rota  o
    mat4 rotationMatrix;
    rotationMatrix(0, 0) = cosTheta + nx * nx * oneMinusCos;
    rotationMatrix(0, 1) = nx * ny * oneMinusCos - nz * sinTheta;
    rotationMatrix(0, 2) = nx * nz * oneMinusCos + ny * sinTheta;
    rotationMatrix(0, 3) = 0.0f;

    rotationMatrix(1, 0) = ny * nx * oneMinusCos + nz * sinTheta;
    rotationMatrix(1, 1) = cosTheta + ny * ny * oneMinusCos;
    rotationMatrix(1, 2) = ny * nz * oneMinusCos - nx * sinTheta;
    rotationMatrix(1, 3) = 0.0f;

    rotationMatrix(2, 0) = nz * nx * oneMinusCos - ny * sinTheta;
    rotationMatrix(2, 1) = nz * ny * oneMinusCos + nx * sinTheta;
    rotationMatrix(2, 2) = cosTheta + nz * nz * oneMinusCos;
    rotationMatrix(2, 3) = 0.0f;

    rotationMatrix(3, 0) = 0.0f;
    rotationMatrix(3, 1) = 0.0f;
    rotationMatrix(3, 2) = 0.0f;
    rotationMatrix(3, 3) = 1.0f;

    return rotationMatrix;

}

inline mat4 lookAt(vec3 eye, vec3 center, vec3 up)
{
	vec3 f = normalize(center - eye);
	vec3 s = normalize(cross(f, normalize(up)));
	vec3 u = cross(s, f);

	Mat Vt = transpose(toMat(s, u, -f));

	Mat view = toMat4(Vt) * translate(-eye);

	return view;
}

inline mat4 orthogonal(float l, float r, float b, float t, float n, float f){
	return {
		2/(r-l),      0,     0,      (l+r)/(l-r),
			0,  2/(t-b),     0,      (b+t)/(b-t),
			0,        0, -2/(f-n),   (n+f)/(n-f),
			0,        0,      0,               1
	};	
}

inline mat4 frustum(float l, float r, float b, float t, float n, float f){
	mat4 Persp = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0,-(n+f)/n, -f,
		0, 0, -1/n, 0
	};
	mat4 Volume = {
		2/(r-l),      0,     0,      (l+r)/(l-r),
			0,  2/(t-b),     0,      (b+t)/(b-t),
			0,        0, 2/(f-n),    (n+f)/(n-f),
			0,        0,      0,               1
	};	
	return Volume*Persp;
}

inline mat4 perspective(float fovy, float aspect, float Near, float Far){
	float fovyRad = fovy * 3.1415 / 180;
	float t = Near * tan(fovyRad / 2);
	float b = -t;
	float r = t * aspect;
	float l = -r;
	return frustum(l, r, b, t, Near, Far);
}

#endif
