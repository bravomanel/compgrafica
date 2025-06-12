#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "matrix.h"
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

	GLFWwindow* window = glfwCreateWindow(400, 400, "Janela", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);	
	glewInit();
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    const char* source_vs =
        "#version 330\n                 "
        "uniform mat4 M;                "
        "layout (location=0) in vec4 v; "
        "void main(){                   "
        "    gl_Position = M*v;         "
        "}                              ";
        
    const char* source_fs =
        "#version 330\n                  "
        "uniform vec3 C;                 "
        "out vec4 fragColor;             "
        "void main(){ 	                 "
        "    fragColor = vec4(C, 1);     "
        "}                               ";
        
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source_vs, NULL);
    glCompileShader(vertexShader);
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source_fs, NULL);
    glCompileShader(fragmentShader);
        
    unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
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

    unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo_position;
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glViewport(0, 0, 400, 400);
	
	while(!glfwWindowShouldClose(window)){
        glUseProgram(shaderProgram);
        mat4 Model = rotate_y(0.2)*rotate_x(0.1)*translate(-0.5, -0.5, -0.5);
        mat4 View = lookAt({0, 1, 2}, {0, 0, 0}, {0, 1, 0});
    	mat4 Projection = perspective(60, 1, 0.01, 100.0);
        //mat4 Projection = orthogonal(-2, 2, -2, 2, -4, 4);

        mat4 M = Projection*View*Model;
        int loc = glGetUniformLocation(shaderProgram, "M");
        glUniformMatrix4fv(loc, 1, true, M.v);

        vec3 C = {0, 0, 1};
        loc = glGetUniformLocation(shaderProgram, "C");
        glUniform3fv(loc, 1, C.v);

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();    
	}
	glfwTerminate();
}
