#include "Image.h"
#include "raytracing.h"
#include "Phong.h"

struct Material{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Ns;
};

struct Scene{
    ImageRGB            image;
    Camera              camera;
    Light               light;
    Material            material;
    std::vector<Sphere> spheres;

    void render(){
        #pragma omp parallel for schedule(dynamic, 1)  // OpenMP
        for(int y = 0; y < image.height(); y++)
            for(int x = 0; x < image.width(); x++)
                image(x, y) = color_at(camera.ray(x,y));
    }

    RGB color_at(Ray ray) const{
        auto intersection = min_intersection(ray, spheres);

        if(intersection.t == HUGE_VALF)
            return white;

        vec3 position = ray.at(intersection.t);
        vec3 normal = intersection.sphere.normal(position);
        return illumination(position, normal, light, material);
    }
};

int main(){
    int w = 800;
    int h = 600;
    float R = 1e5;

    Scene scene{
        ImageRGB{w, h},
        Camera{0.1, 45*M_PI/180, w, h},
        Light{
            vec4{0.0, 13, 15, 1.0}, // position
	        vec3{0.2, 0.2, 0.2}, // Ia
	        vec3{1.0, 1.0, 1.0}, // Id
	        vec3{1.0, 1.0, 1.0}  // Is
        },
        Material{
            vec3{1.0, 1.0, 1.0}, // Ka
            vec3{0.5, 0.5, 0.5}, // Kd
            vec3{0.5, 0.5, 0.5}, // Ks
            128 // Ns
        },
        {// spheres
            Sphere{R, {R+10, 0, 0}},
            Sphere{R, {-R-10, 0, 0}},
            Sphere{R, {0, R+10, 0}},
            Sphere{R, {0, -R-10, 0}},
            Sphere{R, {0, 0, -R-30}},
            Sphere{3, {4, -7, -20}},
            Sphere{5, {-3, -7, -15}},
            Sphere{4, {-4, 3, -22}},
            Sphere{2, {-4, -7, 2}}
        }
    };
	scene.camera.lookAt({0, 3, 30}, {0, 0, 0}, {0, 1, 0});
    scene.render();
    scene.image.save("output.png");
}