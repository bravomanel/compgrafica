#include <GLFW/glfw3.h>

#include "Render3D.h"
#include "ZBuffer.h"
#include "TextureShader.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "ImageSet.h"

class Mesh{
	std::vector<ObjMesh::Vertex> tris;
	std::vector<MaterialRange> materials;
	ImageSet image_set;
	public:
	mat4 Model;

	Mesh(std::string obj_file, mat4 _Model, std::string default_texture = ""){
		ObjMesh mesh{obj_file};
		tris = mesh.getTriangles();

		MaterialInfo std_mat;
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);

		for(MaterialRange range: materials)
			image_set.load_texture(mesh.path, range.mat.map_Kd);

		Model = _Model;
	}
	
	void draw(ImageZBuffer& G, TextureShader& shader) const{
		for(MaterialRange range: materials){
			image_set.get_texture(range.mat.map_Kd, shader.texture.img);
			TrianglesRange T{range.first, range.count};
			Render3D(tris, T, shader, G);
		}
	}
};

int main(){
	glfwInit();
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    int screen_width = 800, screen_height = 600;
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "CG UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	

    Mesh mesh{
		"modelos/metroid/DolBarriersuit.obj", translate(0, -5.5, 0)*scale(0.6, 0.6, 0.6)
		//"modelos/pose/pose.obj", translate(0, -5, 0)*scale(0.05, 0.05, 0.05)
		//"modelos/Wood Table/Old Wood Table.obj", loadIdentity()
		//"modelos/train-toy-cartoon/train-toy-cartoon.obj", translate(0, -4, 0)*scale(300, 300, 300)
	};

    ImageRGB G{screen_width, screen_height};;

    TextureShader shader;
    shader.texture.filter = BILINEAR;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;

	mat4 View = lookAt({10, 5, 10}, {0, 0, 0}, {0, 1, 0});
	float a = screen_width/(float)screen_height;
	mat4 Projection = perspective(45, a, 0.1, 100);

    float theta = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;
	
	while(!glfwWindowShouldClose(window)){
        glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
        last_x = xpos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    theta += dx*0.01;
        }

        G.fill(white);
        ImageZBuffer I{G};
            
        mat4 Model = rotate_y(theta)*mesh.Model;
        shader.M = Projection*View*Model;

        mesh.draw(I, shader);
            
        glDrawPixels(screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, G.data());

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

