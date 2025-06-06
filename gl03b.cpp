#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "utilsGL.h"

int main(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(400, 400, "Janela", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);	
	glewInit();
	enable_debug(); 

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	ShaderProgram shaderProgram{
		Shader{"SimpleShader.vert", GL_VERTEX_SHADER},
		Shader{"SimpleShader.frag", GL_FRAGMENT_SHADER}
	};
	
	vec3 V[] = {
		{0, 0, 0},
		{1, 0, 0},
		{1, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{1, 1, 1},
		{0, 1, 1},
	};
	unsigned int indices[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7
	};

	VAO vao{true};
	glBindVertexArray(vao);
    
    GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(V);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	GLBuffer ebo{GL_ELEMENT_ARRAY_BUFFER};
	ebo.data(indices);

	glViewport(0, 0, 400, 400);
	
	while(!glfwWindowShouldClose(window)){
        mat4 Model = rotate_y(0.2)*rotate_x(0.1)*translate(-0.5, -0.5, -0.5);
        mat4 View = lookAt({0, 1, 2}, {0, 0, 0}, {0, 1, 0});
    	mat4 Projection = perspective(60, 1, 0.01, 100.0);
        
        glUseProgram(shaderProgram);
        Uniform{"M"} = Projection*View*Model;
        Uniform{"C"} = vec3{0, 0, 1};

        glClearColor(1, 1, 1, 1);	
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
