#ifndef SCENE_H
#define SCENE_H

#include "GLMesh.h"
#include "transforms.h"

class Scene{
    public:
    std::vector<GLMesh> meshes;

    Scene(){
        meshes.emplace_back(
		"modelos/Retroville Street/bg_jimmy_06_hi.obj",
		translate(0, 1, -5)*rotate_y(0.4)*scale(1,1,1));

	meshes.emplace_back(
		"modelos/Luigi/luigi.obj",
		translate(-3.5, 1, 2.5)*rotate_y(1.4)*scale(0.01, 0.01, 0.01));

	meshes.emplace_back(
		"modelos/Mario/mario.obj",
		translate(-2, 1, 1)*rotate_y(0.7)*scale(0.01, 0.01, 0.01));

	meshes.emplace_back(
		"modelos/Arm Cannon/Light Beam.obj", 
		translate(0.4, 2.65, 1.15)*rotate_y(0.2)*scale(1,1,1));

	meshes.emplace_back(
		"modelos/Samus (PED Suit)/Samus (PED Suit + Hazard Shield, 75% Corruption).obj", 
		translate(1, 1, 1)*rotate_y(0.2)*scale(0.6,0.6,0.6));

	meshes.emplace_back(
		"modelos/Kawasaki_Ki-61/14082_WWII_Plane_Japan_Kawasaki_Ki-61_v1_L2.obj", 
		translate(0, 2, -4)*rotate_x(-1.76)*rotate_z(-1.57)*scale(5, 5, 5));
    }

    void draw() const{
        for(const GLMesh& mesh: meshes)
            mesh.draw();
    }

    private:
    SurfaceMesh flag_mesh(int m, int n) const{
        int N = m*n;

        float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
            v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

        SurfaceMesh res;
        res.vertices.resize(N);
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                float u = u0 + i*du;
                float v = v0 + j*dv;

                Vertex& V = res.vertices[i + j*m];
                V.position = {u, v, sinf(u*v/4)};

                float s = i/(m-1.0);
                float t = j/(n-1.0);

                V.texCoords = {s, t};

                vec3 du = {1, 0, cosf(u*v/4)*v/4};
                vec3 dv = {0, 1, cosf(u*v/4)*u/4};
                V.normal = normalize(cross(du, dv));
            }
        }

        for(int i = 0; i < m-1; i++){
            for(int j = 0; j < n-1; j++){	
                unsigned int ij = i + j*m;
                res.indices.insert(res.indices.end(), {
                        ij, ij+1, ij+m,
                        ij+m+1, ij+m, ij+1
                        });
            }
        }

        return res;
    }
};

#endif
