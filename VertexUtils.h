#ifndef VERTEXUTILS_H
#define VERTEXUTILS_H

#include "vec.h"
#include "Color.h"

template <class Vec>
struct PosCol{
	Vec position;
	RGB color;
};

using Vec2Col = PosCol<vec2>;
using Vec3Col = PosCol<vec3>;
using Vec4Col = PosCol<vec4>;

#endif
