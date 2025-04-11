#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <cstddef>
#include <array>
#include <vector>

template<class Prims, class Cont>
auto assemble(const Prims& P, const Cont& V)
{
	using Primitive = decltype(P.assemble(0, std::data(V)));
	std::vector<Primitive> res(P.n);
	for(unsigned int i = 0; i < P.n; i++)
		res[i] = P.assemble(i, std::data(V));
	return res;
}

///////////////////////////////////////////////////////////////////////
template<class V>
using Line = std::array<V, 2>;


struct BasePrimitive{
	size_t n;
};

struct Lines : BasePrimitive{
	Lines(size_t n_verts){ n = n_verts/2; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[2*i], V[2*i+1] };
	}
};

struct LineStrip : BasePrimitive{
	LineStrip(size_t s){ n = s>1? s-1: 0; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[i], V[i+1] };
	}
};

struct LineLoop : BasePrimitive{
	LineLoop(size_t s){ n = s; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[i], V[(i+1)%n] };
	}
};

///////////////////////////////////////////////////////////////////////
template<class V>
using Triangle = std::array<V, 3>;

struct Triangles : BasePrimitive{
	Triangles(size_t n_verts){ n = n_verts/3; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[3*i], V[3*i + 1], V[3*i + 2] };
	}
};

struct TriangleStrip : BasePrimitive{
	TriangleStrip(size_t s){ n = s>2? s-2: 0; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[i], V[i+1], V[i+2] };
	}
};

struct TriangleFan : BasePrimitive{
	TriangleFan(size_t s){ n = s>2? s-2: 0; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[0], V[i+1], V[i+2] };
	}
};

struct TrianglesRange : BasePrimitive{
	unsigned int first;

	TrianglesRange(unsigned int first, unsigned int s): first{first}{ n = s/3; }

	template<typename Vertex>
	Triangle<Vertex> assemble(unsigned int i, const Vertex* V) const{
		return { V[first + 3*i], V[first + 3*i + 1], V[first + 3*i + 2] };
	}
};

///////////////////////////////////////////////////////////////////////
template<class Primitives>
struct Elements : BasePrimitive{
	private:
	const unsigned int* indices;
	Primitives P;
	
	public:
	template<class Indices>
	Elements(const Indices& in): indices{std::data(in)}, P{std::size(in)}{ n = P.n; }

	template<typename Vertex>
	auto assemble(unsigned int i, const Vertex* V) const{
		return assemble(P.assemble(i, indices), V);
	}
	
	template<typename Vertex>
	Line<Vertex> assemble(Line<unsigned int> indices, const Vertex* V) const{
		return { V[indices[0]], V[indices[1]] };
	}
	
	template<typename Vertex>
	Triangle<Vertex> assemble(Triangle<unsigned int> indices, const Vertex* V) const{
		return { V[indices[0]], V[indices[1]], V[indices[2]] };
	}
};

///////////////////////////////////////////////////////////////////////
template<class Primitives>
struct TriLines : BasePrimitive{
	private:
	Primitives P;

	public:
	template<class... Args>
	TriLines(Args&&... args) : P{std::forward<Args>(args)...}{ n = 3*P.n; }

	template<typename Vertex>
	Line<Vertex> assemble(unsigned int i, const Vertex* V) const{
		unsigned j = i/3;
		unsigned k = i%3;

		Triangle<Vertex> tri = P.assemble(j, V);

		return { tri[k], tri[(k+1)%3] };
	}
};

#endif
