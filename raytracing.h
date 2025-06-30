#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "Primitives.h"
#include "vec.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////
struct Ray{
    vec3 orig;
    vec3 dir;

    vec3 at(float t) const{
        return orig + t*dir;
    }
}; 

Ray operator*(mat4 M, Ray ray){
    return {
        toVec3(M*toVec4(ray.orig, 1)),
        toVec3(M*toVec4(ray.dir, 0)),
    };
}

////////////////////////////////////////////////////////////////////
template<typename Range>
auto min_intersection(Ray ray, const Range& range){
    decltype(intersection(ray, *std::begin(range))) result;
    for(const auto& x: range)
        result = std::min(result, intersection(ray, x));
    return result;
}

////////////////////////////////////////////////////////////////////
class Camera{
    // extrinsic parameters
    vec3 position = vec3{0, 0, 0};
    vec3 direction = vec3{0, 0, -1};
    vec3 up = vec3{0, 1, 0};
    vec3 right = vec3{1, 0, 0};

    // intrinsic parameters
    float near;
    float fovy;
    int w, h;

    public:
    Camera(float near, float fovy, int w, int h) : 
        near{near},fovy{fovy},w{w},h{h}{}

    void lookAt(vec3 pos, vec3 target, vec3 _up){
        position = pos;
        direction = normalize(target - position);
        right = normalize(cross(direction, _up));
        up = cross(right, direction);
    }

    Ray ray(float i, float j) const{
        float ymax = near*tan(fovy/2);
        float xmax = (ymax*w)/h;
        float x = 2*xmax*(i+0.5f)/w - xmax;
        float y = 2*ymax*(j+0.5f)/h - ymax;
        return {position, near*direction + x*right + y*up};
    }
};

////////////////////////////////////////////////////////////////////
struct Intersection{
    float t = HUGE_VALF;
    bool operator<(const Intersection& other) const{
        return t < other.t;
    }
};

////////////////////////////////////////////////////////////////////

// Menor solucao positiva da equacao ax^2 + bx + c = 0
// Retorna um valor negativo se nao houver solucao positiva
inline float smallerT(float a, float b, float c){
	float delta = b*b - 4*a*c;
	const float EPS = 1e-50;
	if(delta < -EPS)
		return -1.0;

	if(delta < EPS){
		float t = -b/(2*a);
		return t;
	}

    float t1 = (-b-sqrt(delta))/(2*a);
    float t2 = (-b+sqrt(delta))/(2*a);

    if(t1 < 0)
        return t2;

    if(t2 < 0)
        return t1;

    return std::min(t1, t2);
}

////////////////////////////////////////////////////////////////////

struct Sphere{
    float radius;
    vec3 center;

    vec3 normal(vec3 pos) const{
        return pos - center;
    }
};

struct SphereIntersection : public Intersection{
    Sphere sphere;
};

inline SphereIntersection intersection(Ray ray, Sphere sphere){
    vec3 dif = ray.orig - sphere.center;
    vec3 dir = ray.dir;
    
    float a = dot(dir, dir);
    float b = 2*dot(dif, dir);
    float c = dot(dif, dif) - sphere.radius*sphere.radius;
    
    float t = smallerT(a, b, c);
    return {t < 1e-50? HUGE_VALF: t, sphere};
}

inline SphereIntersection iuuuntersection(Ray ray, Sphere sphere){
    return intersection(ray, sphere);
}

////////////////////////////////////////////////////////////////////


template<class Vertex>
struct TriangleIntersection : public Intersection{
    float u;
    float v;
    const Triangle<Vertex>* triangle = nullptr;
};


inline bool intersect(Ray ray, const Triangle<vec3>& P, float &t, float &u, float &v){ 
    // Algoritmo de Moller-Trumbore
    // Resolver o sistema: [E1 E2 -D][u v t]^T = C0
    // onde E1 = P1 - P0
    //      E2 = P2 - P0
    //      C0 =  C - P0

    vec3 dir = ray.dir;
    vec3 orig = ray.orig;

    vec3 E1 = P[1] - P[0]; 
    vec3 E2 = P[2] - P[0]; 
    vec3 DxE2 = cross(dir, E2); 
    float det = dot(E1, DxE2); 

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 1e-15) 
        return false; 

    float invDet = 1/det; 
    vec3 C0 = orig - P[0]; 

    // u =  det(C0, E2, -D) / det(E1, E2, -D) 
    //   = (C0 . (D x E2) ) / (E1 . (D x E2) )
    u = dot(C0, DxE2)*invDet; 
    if (u < 0 || u > 1) 
        return false; 

    vec3 C0xE1 = cross(C0, E1); 

    // v =  det(E1, C0, -D) / det(E1, E2, -D) 
    //   = (D . (C0 x E1) ) / (E1 . (D x E2) )
    v = dot(dir, C0xE1)*invDet; 
    if (v < 0 || u + v > 1) 
        return false; 

    // t =  det(E1, E2, C0) / det(E1, E2, -D) 
    //   = (E2 . (C0xE1) ) / (E1 . (D x E2) )
    t = dot(E2, C0xE1)*invDet; 

    return t > 1e-3; 
} 

inline Triangle<vec3> get_triangle(const Triangle<vec3>& tri){
    return tri;
}

template<class Vertex>
TriangleIntersection<Vertex> intersection(Ray ray, const Triangle<Vertex>& triangle){
    Triangle<vec3> tri = get_triangle(triangle);
    TriangleIntersection<Vertex> result;
    result.triangle = &triangle;
    if(!intersect(ray, tri, result.t, result.u, result.v))
        result.t = HUGE_VALF;
    return result;
}
    

////////////////////////////////////////////////////////////////////

class DummyBoundingVolume{
    public:
    DummyBoundingVolume() = default;
    DummyBoundingVolume(const std::vector<vec3>&){}
    bool intersect(Ray) const{ return true; }
};

////////////////////////////////////////////////////////////////////
inline vec3 most_distant_point(const std::vector<vec3>& points, vec3 ref){
    return *std::max_element(points.begin(), points.end(), 
        [&](const vec3& p, const vec3& q){ 
            return norm(p-ref) < norm(q-ref);
        }
    );
}

////////////////////////////////////////////////////////////////////

class BoundingSphere{
    Sphere bounding_sphere;

    public:
    BoundingSphere() = default;
    BoundingSphere(const std::vector<vec3>& position){
        if(position.size() < 1)
            return;

        vec3 py = most_distant_point(position, position[0]);
        vec3 pz = most_distant_point(position, py);

        bounding_sphere.center = 0.5*(py+pz);
        bounding_sphere.radius = 0.5*norm(py-pz);

        auto cposition = position;
        while(!cposition.empty()){
            auto it = std::find_if(cposition.begin(), cposition.end(), 
                [&](const vec3& p){ 
                    return norm(p-bounding_sphere.center) > bounding_sphere.radius; 
                });

            if(it == cposition.end())
                break;
            
            float A = norm(*it - bounding_sphere.center);
            float t = (A - bounding_sphere.radius)/(2*A);
            bounding_sphere.center = lerp(t, bounding_sphere.center, *it);
            bounding_sphere.radius = norm(*it - bounding_sphere.center);
            cposition.erase(it);
        }
    }

    bool intersect(Ray ray) const{
        SphereIntersection I = intersection(ray, bounding_sphere);
        return I.t < HUGE_VALF;
    }
};

////////////////////////////////////////////////////////////////////

class BoundingBox{
    bool init = false;
    vec3 pmin, pmax;
    public:
    BoundingBox() = default;
    
    BoundingBox(const std::vector<vec3>& verts){
        for(vec3 v: verts)
            add(v);
    }

    template<class Iterator>
    BoundingBox(Iterator b, Iterator e){
        for(auto it = b; it != e; it++)
            for(vec3 v: get_triangle(*it))
                add(v);
    }

    vec3 mid_point() const{
        return 0.5*(pmin + pmax);
    }

    void add(vec3 p){
        if(!init){
            pmin = pmax = p;
            init = true;
        }else{
            for(int i = 0; i < 3; i++){
                pmin[i] = std::min(pmin[i], p[i]);
                pmax[i] = std::max(pmax[i], p[i]);
            }
        }
    }

    bool intersect(Ray ray) const{
        float tmin = 0;
        float tmax = INFINITY;
        for(int i = 0; i < 3; i++){
            float dmin = pmin[i] - ray.orig[i];
            float dmax = pmax[i] - ray.orig[i];
            float d = ray.dir[i];
            if(d == 0){
                if(dmin > 0 || dmax < 0)
                    return false;
            }else{
                float a = dmin/d;
                float b = dmax/d;
                if(b < a)
                    std::swap(a, b);
        
                if(a > tmin)
                    tmin = a;
                if(b < tmax)
                    tmax = b;
            }
        }
        return tmin <= tmax;
    }
};

////////////////////////////////////////////////////////////////////

template<class Vertex>
struct TriRange{
    using Tri             = Triangle<Vertex>;
    using Iterator        = typename std::vector<Tri>::iterator;
    using TriIntersection = TriangleIntersection<Vertex>;

    Iterator b, e;
    
    Iterator begin() const{ return b; }
    Iterator end()   const{ return e; }
    
    friend TriIntersection intersection(Ray ray, const TriRange& range){
        return min_intersection(ray, range);
    }
};

////////////////////////////////////////////////////////////////////
template<class Vertex>
class Octree{
    using Tri             = typename TriRange<Vertex>::Tri;
    using Iterator        = typename TriRange<Vertex>::Iterator;
    using TriIntersection = typename TriRange<Vertex>::TriIntersection;
    TriRange<Vertex> range;

    int level;
    BoundingBox bounding_box;
    std::vector<Octree> children;

    public:
    Octree() = default;

    Octree(Iterator _b, Iterator _e, int level = 4) : 
        range{_b, _e}, level{level}, bounding_box{_b, _e}
    {
        if(level > 0)
            partition(0, bounding_box.mid_point(), _b, _e);
    }
    
    Octree(Octree&&) noexcept = default;
    Octree& operator=(Octree&&) noexcept = default;
    // Disallow copying as it would invalidate iterators used in Octree
    Octree(const Octree&) = delete;
    Octree& operator=(const Octree&) = delete;

    friend TriIntersection intersection(Ray ray, const Octree& octree){
        if(!octree.bounding_box.intersect(ray))
           return TriIntersection{};
        
        if(octree.children.empty())
            return min_intersection(ray, octree.range);
        else
            return min_intersection(ray, octree.children);
    }
    
    private:
    void partition(int i, vec3 c, Iterator beg, Iterator end){
        Iterator it = std::partition(beg, end, 
            [&](const Tri& tri){ 
                Triangle<vec3> T = get_triangle(tri);
                return T[0][i] < c[i] && 
                       T[1][i] < c[i] && 
                       T[2][i] < c[i] ;
            }
        );
        if(i < 2){ 
            partition(i+1, c, beg, it);
            partition(i+1, c, it, end);
        }else{
            add_child(beg, it);
            add_child(it, end);
        }
    }

    void add_child(Iterator beg, Iterator end){
        if(beg != end)
            children.emplace_back(beg, end, level-1);
    }
};

#endif