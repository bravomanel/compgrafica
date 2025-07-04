#include "Image.h"
#include "raytracing.h"
#include "transforms.h"
#include "Phong.h"

//#define USE_BOUNDING_SPHERE
#define USE_BOUNDING_BOX
#define USE_OCTREE
#include "RTMesh.h"

void show_progress(int current_value, int max_value){
    float percent = (float)current_value/(float)max_value*100;
    fprintf(stderr, "\r%.1f%%", percent);
}

struct Scene{
    ImageRGB image;
    Camera camera;
    Light light;
    std::vector<RTMesh> meshes;

    void render(){
        #pragma omp parallel for schedule(dynamic, 1)  // OpenMP
        for(int y = 0; y < image.height(); y++){
            show_progress(y, image.height()-1);
            for(int x = 0; x < image.width(); x++)
                image(x, y) = color_at(camera.ray(x,y));
        }
    }

    RGB color_at(Ray ray) const{
        MatTriIntersection I = min_intersection(ray, meshes);
        if(I.t == HUGE_VALF)
            return white;

        sample_textures(I.material, I.texCoords);

        return illumination(I.position, I.normal, light, I.material);
    }
};
    
MaterialInfo get_material(RGB base_color){
    MaterialInfo mat = standard_material();
    vec3 color = toVec(base_color);
    mat.Ka = color;
    mat.Kd = color;
    mat.Ks = color;
    return mat;
}

std::vector<RTMesh> get_meshes(){
    std::vector<RTMesh> meshes;
    meshes.emplace_back("modelos/monkey.obj", translate(0, 5.6, -2)*scale(1.4, 1.4, 1.4)*rotate_x(-0.7));
    meshes.emplace_back("modelos/bunny.obj", translate(0, 5.2, 2), get_material(blue));
    meshes.emplace_back("modelos/teapot.obj", translate(6,0,4)*scale(.14,.14,.14)*rotate_x(-M_PI/2), get_material(yellow));
    meshes.emplace_back("modelos/wall.obj", scale(20, 20, 20), get_material(green));
    meshes.emplace_back("modelos/Wood Table/Old Wood Table.obj", translate(0,1.08,0));
    meshes.emplace_back("modelos/pose/pose.obj", translate(-6, 0, 4)*rotate_y(1)*scale(.05, .05, .05));
    meshes.emplace_back("modelos/train-toy-cartoon/train-toy-cartoon.obj", translate(0,0,6)*rotate_y(-2.3)*scale(120, 120, 120));
    return meshes;
}

int main(){
    int w = 600;
    int h = 500;

    Scene scene{
        ImageRGB{w, h},
        Camera{0.1, 40*M_PI/180, w, h},
        Light{
            vec4{0.0, 13, 15, 1.0}, // position
	        vec3{0.2, 0.2, 0.2}, // Ia
	        vec3{1.0, 1.0, 1.0}, // Id
	        vec3{1.0, 1.0, 1.0}  // Is
        },
        get_meshes()
    };
	scene.camera.lookAt({10, 7, 20}, {0, 4, 0}, {0, 1, 0});
    scene.render();
    scene.image.save("output.png");
}