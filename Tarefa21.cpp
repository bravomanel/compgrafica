#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "ObjMesh.h"
#include "SceneTarefa.h"
#include "Shaders.h"
	
class Skybox{
	ShaderProgram shaderProgram;
	GLTexture cubemap;
	VAO vao;
	GLBuffer vbo;
	int n_verts;

	public:
	Skybox(std::string files[6]){
		shaderProgram = ShaderProgram{
			Shader{"cubemap.vert", GL_VERTEX_SHADER},
			Shader{"cubemap.frag", GL_FRAGMENT_SHADER}
		};
		init_cubemap(files);
		
		init_mesh("modelos/box.obj");
		
	}

	void draw(mat4 Projection, mat4 View) const{
		// Apaga translação
		View(0, 3) = View(1,3) = View(2,3) = 0;

		glUseProgram(shaderProgram);
		Uniform{"Projection"} = Projection;
		Uniform{"View"} = View;
		Uniform{"Model"} = loadIdentity();

		glDepthMask(GL_FALSE);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, n_verts);
		glDepthMask(GL_TRUE);
	}

	private:
	void init_cubemap(std::string files[6]){
		cubemap = GLTexture{GL_TEXTURE_CUBE_MAP};

		cubemap.load(files[0], GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		cubemap.load(files[1], GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		cubemap.load(files[2], GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		cubemap.load(files[3], GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		cubemap.load(files[4], GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		cubemap.load(files[5], GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void init_mesh(std::string filename){
		ObjMesh mesh{filename};
		auto mesh_triangles = mesh.getTriangles();
		
		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(mesh_triangles);

		size_t stride = sizeof(ObjMesh::Vertex);
		size_t offset_position = offsetof(ObjMesh::Vertex, position);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
		
		n_verts = mesh_triangles.size();
	}
};

int main(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
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

	glewInit();
	glEnable(GL_DEPTH_TEST);

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	PhongShaderTexNormal shader;
	Scene scene;
	Light light;
	light.position = vec4{ 10.0, 8.0, 10.0, 1.0 };
	light.Ia = vec3{ 0.2, 0.2, 0.2 };
	light.Id = vec3{ 1.0, 1.0, 1.0 };
	light.Is = vec3{ 1.0, 1.0, 1.0 };
	
	std::string files[6] = {
		"skybox/right.png",
		"skybox/left.png",
		"skybox/top.png",
		"skybox/bottom.png",
		"skybox/front.png",
		"skybox/back.png"
	};	
	Skybox skybox{files};

	float angle = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;
	while(!glfwWindowShouldClose(window)){
		glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
        last_x = xpos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    angle += dx*0.01;
        }

        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

		glClearColor(1, 1, 1, 1);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float a = screen_width/(float)screen_height;
		mat4 Projection = perspective(45, a, 0.1, 40);
		vec4 pos = rotate_y(angle)*vec4{0, 2.8, 5, 1};
		mat4 View = lookAt(toVec3(pos), {0, 2.8, 0}, {0, 1, 0});
		
		skybox.draw(Projection, View);
		
		shader.setup(Projection, View, light);
		scene.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

