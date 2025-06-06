#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "Color.h"
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

	//glEnable(GL_DEPTH_TEST);

	ShaderProgram shaderProgram{
		Shader{"ColorShader.vert", GL_VERTEX_SHADER},
		Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
	};
	
    struct Vec3Col{
	    vec3 position;
	    RGB color;
    };
	Vec3Col V[] = {
		{{0, 0, 0}, red},
		{{1, 0, 0}, red},
		{{1, 1, 0}, green},
		{{0, 1, 0}, green},
		{{0, 0, 1}, blue},
		{{1, 0, 1}, blue},
		{{1, 1, 1}, yellow},
		{{0, 1, 1}, yellow}
	};

    unsigned int indices[] = {
       0, 1, 5,   0, 5, 4,
       0, 3, 7,   0, 7, 4,
       0, 1, 2,   0, 2, 3,
       4, 5, 7,   6, 7, 5,
       5, 1, 6,   2, 6, 1,
       6, 2, 7,   3, 7, 2,
    };


	VAO vao{true};
	glBindVertexArray(vao);

	GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(V);
	
	size_t stride = sizeof(Vec3Col);
	size_t offset_position = offsetof(Vec3Col, position);
	size_t offset_color = offsetof(Vec3Col, color);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride,(void*)offset_color);
	
	GLBuffer ebo{GL_ELEMENT_ARRAY_BUFFER};
	ebo.data(indices);
    
    float angle = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;
	
	while(!glfwWindowShouldClose(window)){
        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        
        glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
        last_x = xpos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    angle += dx*0.01;
        }

        glClearColor(1, 1, 1, 1);	
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 Model = rotate_y(angle)*translate(-0.5, -0.5, -0.5);
        mat4 View = lookAt({1.5, 1.5, 1.5}, {0, 0, 0}, {0, 1, 0});
        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(60, a, 0.1, 5);

        glUseProgram(shaderProgram);
        Uniform{"M"} = Projection*View*Model;

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
