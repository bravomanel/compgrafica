#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "utilsGL.h"
#include "transforms.h"

struct RayTracing{
	VAO vao;
	ShaderProgram shaderProgram;
	int screen_width = 600;
	int screen_height = 600;

	// Controle da rotacao
	mat4 R = loadIdentity();
	mat4 BaseView = lookAt({0, 0, 5}, {0, 0, 0}, {0, 1, 0});
	float vangle = 0;

	int shadow_type = 0;

	RayTracing(){
		glEnable(GL_DEPTH_TEST); 
		
		shaderProgram = ShaderProgram{
			Shader{"RayTracing.vert", GL_VERTEX_SHADER},
			Shader{"RayTracing.frag", GL_FRAGMENT_SHADER}
		};

		vao = VAO{true};
	}

	void draw(){
		glViewport(0, 0, screen_width, screen_height);

		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = (float)screen_width/(float)screen_height;

		glUseProgram(shaderProgram);
		Uniform{"Projection"} = scale(1, 1, -1)*perspective(70, aspect, .5, 50);
		Uniform{"View"} = rotate_x(vangle)*BaseView;
		Uniform{"ShadowType"} = shadow_type;

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void update_angles(float dx, float dy){
		vangle += 0.01*dy;
		vangle = clamp(vangle, -1.5, 1.5);
		BaseView = rotate_y(dx*0.01)*BaseView;
	}

	void update_translation(float xmove, float zmove){
        BaseView = translate(xmove, 0, zmove)*BaseView;
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

	RayTracing scene;

	double last_x, last_y;
    glfwGetCursorPos(window, &last_x, &last_y);
	
	while(!glfwWindowShouldClose(window)){
        glfwGetWindowSize(window, &scene.screen_width, &scene.screen_height);

		// Verifica mouse
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			scene.update_angles(xpos - last_x, ypos - last_y);
		last_x = xpos;
		last_y = ypos;

        // Verifica teclado
	    float zmove = 0;
	    float xmove = 0;
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            zmove += 0.2;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            zmove -= 0.2;
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            xmove += 0.2;        
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            xmove -= 0.2;  
		scene.update_translation(xmove, zmove);

		if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			scene.shadow_type = 0;
		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			scene.shadow_type = 1;
		if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			scene.shadow_type = 2;

		scene.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
