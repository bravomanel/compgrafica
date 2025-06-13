#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "ObjMesh.h"
#include "transforms.h"

class GLMesh{
	VAO vao;
	GLBuffer vbo;
    std::vector<MaterialRange> materials;
	public:

	GLMesh(std::string obj_file){
		ObjMesh mesh{obj_file};

        MaterialInfo standard_material;
        standard_material.name = "standard";
        standard_material.Ka = {1, 1, 1};
        standard_material.Kd = {1, 1, 1};
        standard_material.Ks = {0, 0, 0};
        standard_material.Ns = 1;
        materials = mesh.getMaterials(standard_material);

		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data( mesh.getTriangles() );

		size_t stride = sizeof(ObjMesh::Vertex);
		size_t offset_position = offsetof(ObjMesh::Vertex, position);
		size_t offset_texCoords = offsetof(ObjMesh::Vertex, texCoords);
		size_t offset_normal = offsetof(ObjMesh::Vertex, normal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,(void*)offset_texCoords);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)offset_normal);
	}

	void draw() const{
		glBindVertexArray(vao);
        for(MaterialRange range: materials){
            Uniform{"Ka"} = range.mat.Ka; 
            Uniform{"Kd"} = range.mat.Kd;
            Uniform{"Ks"} = range.mat.Ks;
            Uniform{"shininess"} = range.mat.Ns;

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, range.first, range.count);
        }
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
    
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    
	glEnable(GL_DEPTH_TEST);

	ShaderProgram shaderProgram{
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
		//Shader{"GouraudShader.vert", GL_VERTEX_SHADER},
		//Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
	};

    GLMesh mesh{"modelos/train-toy-cartoon/train-toy-cartoon.obj"};

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
        mat4 Projection = perspective(45, a, 0.1, 50);
        mat4 View = lookAt({1, 1, 1}, {0, 0.3, 0}, {0, 1, 0});
        mat4 Model = rotate_y(angle)*scale(40, 40, 40);

        glUseProgram(shaderProgram);
        Uniform{"Projection"} = Projection;
        Uniform{"View"} = View;
        Uniform{"Model"} = Model;
        
        Uniform{"light_position"} = vec4{ 0, 13, 15, 1.0};
        Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
        Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
        Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

        mesh.draw();
            
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

