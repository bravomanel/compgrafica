#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "VertexUtils.h"
#include "MixColorShader.h"
#include "MarchingCubes.h"
#include "transforms.h"

struct Metaball{
	float a;
	float b;
	vec3 center;

	float operator()(vec3 P){
		float r = norm(P - center);

		return 
		    r >= b   ? 0.0f : 
		    r >= b/3 ? 3*a/2*pow(1 - r/b, 2): 
			           a*(1 - 3*r*r/(b*b));
	}
};

struct MetaballFigure{
	float T;
	std::vector<Metaball> metaballs;

	float operator()(float x, float y, float z){
		vec3 P = {x, y, z};

		float sum = 0;
		for (Metaball fi : metaballs)
			sum += fi(P);

		return T - sum;
	}
};

/*float sphere(float x, float y, float z){
	float r = 1.5;
	return x*x + y*y + z*z - r*r;
}

float torus(float x, float y, float z){
	float R = 1.5;
	float r = 0.4;
	return pow(sqrt(x*x + y*y) - R, 2) + z*z - r*r;
}

float treco(float x, float y, float z){
	float r = (x*x + y*y);
	return 2*y*(y*y - 3*x*x)*(1-z*z) + r*r - (9*z*z - 1)*(1 - z*z);
}*/

int main(){
	MetaballFigure foguete;
	foguete.T = 0.5;
    foguete.metaballs = {
		{1, 1, { -0.6, 0, 0}},
		{1, 0.9, { 0.1, 0, 0}},
		{1, 0.8, { 0.6, 0, 0}},
		{1, 0.4, {0.9, 0, 0}},
		{1, 0.2, {1.1, 0, 0}},
		{1, 0.3, {-0.7, 0,0.6}},
		{1, 0.3, {-0.7, 0,-0.6}},
		{1, 0.3, {-0.7, 0.6,0}},
		{1, 0.3, {-0.7, -0.6,0}},
		{1, 0.2, {-0.9, 0,0.7}},
		{1, 0.2, {-0.9, 0,-0.7}},
		{1, 0.2, {-0.9, 0.7,0}},
		{1, 0.2, {-0.9, -0.7,0}},
    };
    
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
		Shader{"MixColorShader.vert", GL_VERTEX_SHADER},
		Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);
	vec3 pmin = {-2, -2, -2}; 
	vec3 pmax = { 2,  2,  2};

	Uniform{"pmin"} = pmin;
	Uniform{"pmax"} = pmax;
	Uniform{"C"} = {
		toVec(red),    toVec(blue)
	};
	
	std::vector<vec3> V = marchingCubes(foguete, 50, 50, 50, pmin, pmax);

	VAO vao{true};
	glBindVertexArray(vao);

	GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(V);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
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
        mat4 View = lookAt({3, 3, 1.5}, {0, 0, 0}, {0, 0, 1});
        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(50, a, 0.1, 50);

        Uniform{"M"} = Projection*View*Model;
	
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, V.size());

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
