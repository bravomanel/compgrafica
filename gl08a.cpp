#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Image.h"
#include "utilsGL.h"
#include "transforms.h"

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
    enable_debug();
	
    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    glEnable(GL_DEPTH_TEST);
	
	ShaderProgram shaderProgram{
		Shader{"TextureShader.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};

	struct Vertex{
		vec3 position;
		vec2 texCoords;
	};

	Vertex V[] = {
		{{0, 0, 0}, {0, 0}},
		{{10, 0, 0}, {1, 0}},
		{{10, 10, 0}, {1, 1}},
		{{0, 10, 0}, {0, 1}}
	};
	
	VAO vao{true};
	glBindVertexArray(vao);

	GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(V);
	
	size_t stride = sizeof(Vertex);
	size_t offset_position = offsetof(Vertex, position);
	size_t offset_texCoords = offsetof(Vertex, texCoords);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset_texCoords);

#if 1
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        
    ImageRGB img{"minichecker.png"};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0,
                    GL_RGB, GL_UNSIGNED_BYTE, img.data());
#else
	GLTexture texture{GL_TEXTURE_2D};
	texture.load("minichecker.png");
#endif
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

        mat4 Model = rotate_y(angle);
        mat4 View = lookAt({5, 5, 15}, {5, 5, 0}, {0, 1, 0});
        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(50, a, 0.1, 50);

        glUseProgram(shaderProgram);
        Uniform{"M"} = Projection*View*Model;

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
