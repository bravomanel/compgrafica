#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "utilsGL.h"
#include "VertexUtils.h"

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
		Shader{"ColorShader.vert", GL_VERTEX_SHADER},
		Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);

    int m = 50;
    int n = 50;
	int N = m*n;

	float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
          v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

	std::vector<Vec3Col> V(N);
	for(int i = 0; i < m; i++){
		for(int j = 0; j < n; j++){
			float u = u0 + i*du;
			float v = v0 + j*dv;

			int ij = i + j*m;
			V[ij].position = {u, v, sinf(u*v/4)};
			
			float s = i/(m-1.0);
			float t = j/(n-1.0);
			V[ij].color = bilerp(s, t, red, blue, green, yellow);
		}
	}

	std::vector<unsigned int> indices;
	for(int i = 0; i < m-1; i++){
		for(int j = 0; j < n-1; j++){	
			unsigned int ij = i + j*m;
			indices.insert(indices.end(), {
				ij, ij+1, ij+m,
				ij+m+1, ij+m, ij+1
			});
		}
	}

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

        mat4 Model = rotate_z(angle);
        mat4 View = lookAt({10, 10, 10}, {0, 0, 0}, {0, 0, 1});
        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(50, a, 0.1, 50);

        Uniform{"M"} = Projection*View*Model;

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

