#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Color.h"
#include "utilsGL.h"
#include "transforms.h"

int main(){
	unsigned int screen_width = 230;
	unsigned int screen_height = 230;

	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "CG - UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	glewInit();
	enable_debug();
	glEnable(GL_DEPTH_TEST);
	
	ShaderProgram shaderProgram{
		Shader{"QuadShader.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};

	VAO vao{true};

	int selected_texture = 0;

	
	GLTexture texture[2];
	texture[0] = GLTexture{GL_TEXTURE_2D};
	texture[0].load("letra.png");
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	texture[1] = GLTexture{GL_TEXTURE_2D};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);  
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture[1], 0);

	ShaderProgram filterShaderProgram{
		Shader{"QuadShader.vert", GL_VERTEX_SHADER},
		Shader{"MorphFilter.frag", GL_FRAGMENT_SHADER}
	};
	
	glUseProgram(filterShaderProgram);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while(!glfwWindowShouldClose(window)){
		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
			selected_texture = 0;
		}
		if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
			selected_texture = 1;
		}

		glClearColor(1, 1, 1, 1);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, texture[selected_texture]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
		glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

