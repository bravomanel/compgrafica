#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

	while(!glfwWindowShouldClose(window)){
	    glClearColor(1, 1, 1, 1);
	    glClear(GL_COLOR_BUFFER_BIT);
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
