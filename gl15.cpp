#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "transforms.h"

GLTexture init_texture(std::string image){
	GLTexture texture{GL_TEXTURE_2D};
	texture.load(image);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic){
		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
	}
	return texture;
}

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

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	enable_debug();
	glEnable(GL_DEPTH_TEST);
	
	ShaderProgram shaderProgram{
		Shader{"PhongShaderTex.vert", GL_VERTEX_SHADER},
		Shader{"PhongShaderTex.frag", GL_FRAGMENT_SHADER}
	};
	
	struct Vertex{
		vec2 position;
		vec2 texCoords;
		vec3 normal;
	};

	Vertex V[] = {
		{{-5, -5}, {0, 0}, {0, 0, 1}},
		{{ 5, -5}, {1, 0}, {0, 0, 1}},
		{{ 5,  5}, {1, 1}, {0, 0, 1}},
		{{-5,  5}, {0, 1}, {0, 0, 1}}
	};
	
	VAO vao{true};
	glBindVertexArray(vao);

	GLBuffer vbo{GL_ARRAY_BUFFER};
	vbo.data(V);
	
	size_t stride = sizeof(Vertex);
	size_t offset_position = offsetof(Vertex, position);
	size_t offset_texCoords = offsetof(Vertex, texCoords);
	size_t offset_normal = offsetof(Vertex, normal);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset_texCoords);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_normal);
	
	GLTexture diffuse_map  = init_texture("brickwall.jpg");
	GLTexture normal_map = init_texture("brickwall_normal.jpg");

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_map);
	Uniform{"map_Kd"} = 0;
	Uniform{"has_map_Kd"} = true;
		
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	Uniform{"normal_map"} = 1;
	Uniform{"has_normal_map"} = true;

	mat4 Model = loadIdentity();
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
		    Model = rotate_y(dx*0.01)*rotate_x(dy*0.01)*Model;
        }

        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

		glClearColor(1, 1, 1, 1);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 View = lookAt({0, 0, 15}, {0, 0, 0}, {0, 1, 0});
		float a = screen_width/(float)screen_height;
		mat4 Projection = perspective(50, a, 0.1, 50);
		
		Uniform{"Ka"} = vec3{1, 1, 1};
		Uniform{"Kd"} = vec3{1, 1, 1};
		Uniform{"Ks"} = vec3{0.3, 0.3, 0.3};
		Uniform{"shininess"} = 40.0f;

		Uniform{"light_position"} = vec4{ 0.0, 0.0, 10.0, 1.0 };
		Uniform{"Ia"} = vec3{ 0.1, 0.1, 0.1};
		Uniform{"Id"} = vec3{ 0.8, 0.8, 0.8};
		Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

		Uniform{"Projection"} = Projection;
		Uniform{"View"} = View;
		Uniform{"Model"} = Model;
		
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

