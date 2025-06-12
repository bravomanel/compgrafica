#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
        "#version 330\n                  "
        "layout (location=0) in vec4 v;  "
        "void main(){                    "
        "    gl_Position = v;            "
        "}                               ";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &source_vs, NULL);
	glCompileShader(vertexShader);

    const char* source_fs =
        "#version 330\n                        "
        "out vec4 fragColor;                   "
        "void main(){ 	                       "
        "    fragColor = vec4(0, 0, 1, 1);     "
        "}                                     ";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &source_fs, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
    unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float V[6][2] = {
        {-0.8, -0.6},
		{-0.6,  0.8},
		{-0.2, -0.6},
		{ 0.0,  0.0},
		{ 0.6,  0.8},
		{ 0.8, -0.8},
	};

	unsigned int vbo_position;
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glViewport(0, 0, 400, 400);
	
	while(!glfwWindowShouldClose(window)){
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
