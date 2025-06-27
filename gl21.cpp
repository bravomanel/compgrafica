#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "Color.h"
#include "utilsGL.h"

#define PREMULTALPHA

struct Scene2D{
	VAO vao;
	GLBuffer vbo;
	GLTexture background;
	GLTexture boy;
	ShaderProgram shaderProgram;
	int screen_width = 600;
	int screen_height = 600;
		
	Scene2D(){
		glEnable(GL_BLEND);
		
#ifndef PREMULTALPHA
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shaderProgram = ShaderProgram{
			Shader{"TextureShader.vert", GL_VERTEX_SHADER},
			Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
		};
#else
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		
		shaderProgram = ShaderProgram{
			Shader{"TextureShader.vert", GL_VERTEX_SHADER},
			Shader{"TextureShader1.frag", GL_FRAGMENT_SHADER}
		};
#endif
		glUseProgram(shaderProgram);
		
		vec2 V[] = {
			{0, 0},
			{1, 0},
			{1, 1}, 
			{0, 1}, 
		};

		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(V);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		background = GLTexture{GL_TEXTURE_2D};
		background.load("background.png");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		boy = GLTexture{GL_TEXTURE_2D};
		boy.load("flatboy.png");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void draw(){
		glViewport(0, 0, screen_width, screen_height);

		glClearColor(1, 1, 1, 1);	
		glClear(GL_COLOR_BUFFER_BIT);

		float a = screen_width/(float)screen_height;
		mat4 Projection = orthogonal(0, a*10, 0, 10, -1, 1);

		float bg_aspect = 1000.0/750.0;
		mat4 Model = scale(10*bg_aspect, 10, 1);
		Uniform{"M"} = Projection*Model;
		glBindTexture(GL_TEXTURE_2D, background);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		
		Model = translate(3.5, 1.36, 0)*scale(3, 3, 1);
		Uniform{"M"} = Projection*Model;
		glBindTexture(GL_TEXTURE_2D, boy);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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

	GLFWwindow* window = glfwCreateWindow(600, 600, "CG UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	
	glewInit();
	enable_debug();

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	Scene2D scene;

	while(!glfwWindowShouldClose(window)){
        glfwGetWindowSize(window, &scene.screen_width, &scene.screen_height);

		scene.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
