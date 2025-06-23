#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "Scene.h"
#include "Shaders.h"

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
	
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	
	glewInit();
	glEnable(GL_DEPTH_TEST);
	enable_debug();

	// Parâmetros da cena principal
	PhongShaderTexNormal shader;
	Scene scene;
	Light light;
	light.position = vec4{ 0.0, 8.0, 10.0, 1.0 };
	light.Ia = vec3{ 0.2, 0.2, 0.2 };
	light.Id = vec3{ 1.0, 1.0, 1.0 };
	light.Is = vec3{ 1.0, 1.0, 1.0 };

	// Parâmetros da cena secundária
	GLMesh box{"modelos/box.obj", loadIdentity()};
	ShaderProgram textureShader{
		Shader{"TextureShader.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};
	
	unsigned int fwidth = 600, fheight = 600;
	GLTexture box_texture{GL_TEXTURE_2D};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fwidth, fheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic){
		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
	}
	
	unsigned int depthbuffer;
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fwidth, fheight);
	
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);  
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, box_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
	
	mat4 R = loadIdentity();
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;
	double last_y = ypos;
	while(!glfwWindowShouldClose(window)){
		glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
		double dy = ypos - last_y;
        last_x = xpos;
		last_y = ypos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    R = rotate_y(dx*0.01)*rotate_x(dy*0.01)*R;
        }

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, fwidth, fheight);

		glClearColor(0.4, 0.4, 0.6, 1);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		float aspect = (float)fwidth/(float)fheight;
		mat4 Projection = perspective(50, aspect, 1, 50);
		mat4 View = lookAt({0, 10, 20}, {0, 5, 0}, {0, 1, 0})*R;
		shader.setup(Projection, View, light);
		scene.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
		glViewport(0, 0, screen_width, screen_height);

		glClearColor(1, 1, 1, 1);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(textureShader);

		float a = screen_width/(float)screen_height;
		Projection = perspective(45, a, 0.1, 50);
		View = lookAt({0, 2, 5}, {0, 0, 0}, {0, 1, 0});
		Uniform{"M"} = Projection*View*R;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, box_texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		box.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

