#ifndef RT_MESH_H
#define RT_MESH_H

#include "raytracing.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "Sampler2D.h"

inline MaterialInfo standard_material(){
    MaterialInfo info;
    info.name = "standard";
    info.Ka = {0.1f, 0.1f, 0.1f};
    info.Kd = {1.0f, 1.0f, 1.0f};
    info.Ks = {1.0f, 1.0f, 1.0f};
    info.Ns = 12;
    return info;
}

// aliases para facilitar nossa vida
using ObjVertex = ObjMesh::Vertex;
using ObjTriangle = Triangle<ObjVertex>;

struct MatTriIntersection : public Intersection, ObjVertex{
    MaterialInfo material;
};

inline Triangle<vec3> get_triangle(const ObjTriangle& T){
    return { T[0].position, T[1].position, T[2].position };
}

#ifdef USE_OCTREE
using TrianglesVolume = Octree<ObjVertex>;
#else
using TrianglesVolume = TriRange<ObjVertex>;
#endif

class MeshRange{
    MaterialInfo             material;
    std::vector<ObjTriangle> triangles;
    TrianglesVolume          triangles_volume;
   
    public:
    
    MeshRange(MaterialRange range, const std::vector<ObjVertex>& vertices, std::string path){
        material = range.mat;

        material.map_Ka = path + material.map_Ka;
        material.map_Kd = path + material.map_Kd;
        material.map_Ks = path + material.map_Ks;

        TrianglesRange T{range.first, range.count};
        triangles = assemble(T, vertices);

        triangles_volume = {triangles.begin(), triangles.end()};  
    }
    
    friend MatTriIntersection intersection(Ray ray, const MeshRange& mesh_range){
        auto tri_intersection = intersection(ray, mesh_range.triangles_volume);

        MatTriIntersection res;
        res.t = tri_intersection.t;

        if(res.t == HUGE_VALF)
            return res;


        float u = tri_intersection.u;
        float v = tri_intersection.v;
        float w = 1 - (u+v);

        const ObjTriangle& tri = *tri_intersection.triangle;

        res.position  = w*tri[0].position  + u*tri[1].position  + v*tri[2].position;
        res.texCoords = w*tri[0].texCoords + u*tri[1].texCoords + v*tri[2].texCoords;
        res.normal    = w*tri[0].normal    + u*tri[1].normal    + v*tri[2].normal;

        res.material = mesh_range.material;

        return res;
    }
};

/******************************* TEXTURE MANAGEMENT ********************************/
static std::map<std::string, Sampler2D> texture_map;

static Sampler2D* find_texture(std::string filename){
    auto it = texture_map.find(filename);
    return it == texture_map.end()? nullptr: &it->second;
}

// Pre-load texture from file
static void load_texture(std::string path, std::string map_name){
    if(map_name == "")
        return;

    std::string filename = path + map_name;
    if(find_texture(filename))
        return;
    
    std::cout << "load texture " << filename << '\n';
    
    texture_map[filename] = Sampler2D{
        ImageRGB{filename}, BILINEAR, REPEAT, REPEAT
    };
}

// Sample texture using texture coordinates
static void sample_texture(const std::string& filename, vec2 texCoords, vec3& color){
    Sampler2D* sampler = find_texture(filename);
    if(sampler)
        color = toVec(sampler->sample(texCoords));
}

// Sample multiple textures
static void sample_textures(MaterialInfo& mat, vec2 texCoords){
    sample_texture(mat.map_Ka, texCoords, mat.Ka);
    sample_texture(mat.map_Kd, texCoords, mat.Kd);
    sample_texture(mat.map_Ks, texCoords, mat.Ks);

    mat.Ka = mat.Ka * mat.Kd;
}

/**************************** BOUNDING VOLUME *****************************/
#if defined(USE_BOUNDING_BOX)
using BoundingVolume = BoundingBox;
#elif defined(USE_BOUNDING_SPHERE)
using BoundingVolume = BoundingSphere;
#else
using BoundingVolume = DummyBoundingVolume;
#endif


/**************************** RAY TRACING MESH *****************************/
class RTMesh{
    std::vector<MeshRange> mesh_ranges;
    BoundingVolume bounding_volume;
    mat4 M;
    mat4 Mi;
    mat3 MN;
    public:

    RTMesh(const std::string& name, mat4 _M, MaterialInfo std_mat = standard_material()){
        ObjMesh mesh{name};
        M = _M;
        Mi = inverse(M);
        MN = transpose(inverse(toMat3(M)));
        
        auto vertices = mesh.getTriangles();
        
        for(MaterialRange range: mesh.getMaterials(std_mat)){
            mesh_ranges.emplace_back(range, vertices, mesh.path);
            load_texture(mesh.path, range.mat.map_Kd);
            load_texture(mesh.path, range.mat.map_Ka);
            load_texture(mesh.path, range.mat.map_Ks);
        }

        bounding_volume = BoundingVolume{mesh.position};
    }

    friend MatTriIntersection intersection(Ray ray, const RTMesh& mesh){
        MatTriIntersection result;

        // Change ray to model coordinate system
        Ray model_ray = mesh.Mi*ray;
        model_ray.dir = normalize(model_ray.dir);

        if(!mesh.bounding_volume.intersect(model_ray))
            return result;

        result = min_intersection(model_ray, mesh.mesh_ranges);

        if(result.t < HUGE_VALF){
            // Change result to world coordinate system
            vec3& position = result.position;
            position = toVec3(mesh.M*toVec4(position, 1));

            vec3& normal = result.normal;
            normal = mesh.MN*normal;

            result.t = norm(position - ray.orig)/norm(ray.dir);
        }

        return result;
    }
};

#endif