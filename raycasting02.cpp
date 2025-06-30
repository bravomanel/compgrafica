#include <algorithm>
#include "Image.h"
#include "Phong.h"
#include "RTMesh.h"

void show_progress(int current_value, int max_value){
    float percent = (float)current_value/(float)max_value*100;
    fprintf(stderr, "\r%.1f%%", percent);
}

struct Scene{
    ImageRGB image;
    Camera camera;
    Light light;
    RTMesh mesh;

    void render(){
        #pragma omp parallel for schedule(dynamic, 1)  // OpenMP
        for(int y = 0; y < image.height(); y++){
            show_progress(y, image.height()-1);
            for(int x = 0; x < image.width(); x++)
                image(x, y) = color_at(camera.ray(x,y));
        }
    }

    RGB color_at(Ray ray) const{
        MatTriIntersection I = intersection(ray, mesh);
        if(I.t == HUGE_VALF)
            return white;
        
        sample_textures(I.material, I.texCoords);
        
        return illumination(I.position, I.normal, light, I.material);
    }
};

int main(){
    int w = 800;
    int h = 600;

    Scene scene{
        ImageRGB{w, h},
        Camera{0.1, 45*M_PI/180, w, h},
        Light{
            vec4{0.0, 13, 15, 1.0}, // position
	        vec3{0.2, 0.2, 0.2}, // Ia
	        vec3{1.0, 1.0, 1.0}, // Id
	        vec3{1.0, 1.0, 1.0}  // Is
        },
        RTMesh{"modelos/train-toy-cartoon/train-toy-cartoon.obj", rotate_y(-0.6)*scale(100, 100, 100)}
    };

	scene.camera.lookAt({0, 2, 4}, {0, 1, 0}, {0, 1, 0});
    scene.render();
    scene.image.save("output.png");
}