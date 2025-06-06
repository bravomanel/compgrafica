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

	GLFWwindow* window = glfwCreateWindow(800, 600, "CG UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<Mesh> meshes;

	meshes.emplace_back(
		"modelos/floor.obj", 
		scale(35, 35, 35), 
		"../brickwall.jpg");

	meshes.emplace_back(
		"modelos/box.obj", 
		translate(1, 1, -6)*rotate_y(-0.4),
		"../bob.jpg");

	meshes.emplace_back(
		"modelos/metroid/DolBarriersuit.obj", 
		translate(-1, 0, -1)*rotate_y(0.2)*scale(.1, .1, .1));

	meshes.emplace_back(
		"modelos/pose/pose.obj", 
		translate(1, 0, 0)*scale(0.009, 0.009, 0.009));

	meshes.emplace_back(
		"modelos/House Complex/House Complex.obj", 
		translate(4, 0, 0)*rotate_y(0.5*M_PI)*scale(.15, .15, .15));

	meshes.emplace_back(
		"modelos/pony-cartoon/Pony_cartoon.obj", 
		translate(-2, -0.0001, -3)*scale(0.005, 0.005, 0.005),
		"../../blue.png");

    float vangle = 0;
    mat4 BaseView = lookAt({0, 1.6, 5}, {0, 1.6, 0}, {0, 1, 0});

    double last_x, last_y;
    glfwGetCursorPos(window, &last_x, &last_y);
	
	while(!glfwWindowShouldClose(window)){
        // Verifica mouse
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
		double dy = ypos - last_y;
		last_x = xpos;
		last_y = ypos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    vangle += 0.01*dy;
		    vangle = clamp(vangle, -1.5, 1.5);
		    BaseView = rotate_y(dx*0.01)*BaseView;
        }

        // Verifica teclado
	    float zmove = 0;
	    float xmove = 0;
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            zmove += 0.2;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            zmove -= 0.2;
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            xmove += 0.2;        
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            xmove -= 0.2;  
        BaseView = translate(xmove, 0, zmove)*BaseView;

        TextureShader shader;
        shader.texture.filter = BILINEAR;
        shader.texture.wrapX = REPEAT;
        shader.texture.wrapY = REPEAT;

        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        ImageRGB G{screen_width, screen_height};

        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(45, a, 0.1, 1000);
        mat4 View = rotate_x(vangle)*BaseView;

        G.fill(0x00A5DC_rgb);
        ImageZBuffer I{G};

        for(const Mesh& mesh: meshes){
            shader.M = Projection*View*mesh.Model;
            mesh.draw(I, shader);
        }

        glViewport(0, 0, screen_width, screen_height);
        glDrawPixels(screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, G.data());

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
