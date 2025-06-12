#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "VertexUtils.h"
#include "transforms.h"
#include "transforms2d.h"
#include "bezier.h"

int main(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(600, 300, "OpenGL", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	

	glewInit();

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    ShaderProgram shaderProgram{
		Shader{"SimpleShader.vert", GL_VERTEX_SHADER},
		Shader{"SimpleShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);

    std::vector<vec2> CP = loadCurve("curvaC.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 30);

	VAO vao{true};
	glBindVertexArray(vao);

	GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(P);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    float value = 1.3;

	while(!glfwWindowShouldClose(window)){
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            value += 0.1;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            value -= 0.1;

        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

        glClearColor(1, 1, 1, 1);	
        glClear(GL_COLOR_BUFFER_BIT);

        float a = screen_width/(float)screen_height;
        mat4 Projection = orthogonal(0, a, 0, 1, -1, 1);
        mat4 Model = loadIdentity();
        Uniform{"M"} = Projection*Model;
        Uniform{"C"} = vec3{1, 0, 0};

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, P.size());
        
        Model = toMat4(skew(value));
        Uniform{"M"} = Projection*Model;
        Uniform{"C"} = vec3{0, 0, 1};

        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, P.size());


    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
