#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "GLMesh.h"
#include "Scene.h"
#include "Shaders.h"

struct ShadowMap{
	const int SHADOW_WIDTH = 4000, SHADOW_HEIGHT = 4000;
	ShaderProgram shadowShaderProgram;
	GLTexture depthMap;
	unsigned int depthMapFBO;
	mat4 lightSpaceMatrix;

	ShadowMap(){
		//////////////////////////////////////////////////////////////////////////////////////
		// Shader
		shadowShaderProgram = ShaderProgram{
			Shader{"shadow.vert", GL_VERTEX_SHADER},
		};

		//////////////////////////////////////////////////////////////////////////////////////
		// Textura
		depthMap = GLTexture{GL_TEXTURE_2D};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
				SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = {1, 1, 1, 1};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

		//////////////////////////////////////////////////////////////////////////////////////
		// Framebuffer
		glGenFramebuffers(1, &depthMapFBO);  
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		// Associa a textura ao framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

		// Volta ao framebuffer padrão
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void setup(vec4 light_position){
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);

		float a = SHADOW_WIDTH/(float)SHADOW_HEIGHT;
		mat4 Projection = perspective(90, a, 0.1, 50);
		mat4 View = lookAt(toVec3(light_position), {0, 0, 0}, {0, 1, 0});

		glUseProgram(shadowShaderProgram);
		Uniform{"Projection"} = Projection;
		Uniform{"View"} = View;

		lightSpaceMatrix = Projection*View;
	}
};

class PhongShaderShadow{
	ShaderProgram shaderProgram;

	public:
	PhongShaderShadow(){
		shaderProgram = ShaderProgram{
			Shader{"PhongShaderShadow.vert", GL_VERTEX_SHADER},
			Shader{"PhongShaderShadow.frag", GL_FRAGMENT_SHADER}
		};
	}

	void setup(mat4 Projection, mat4 View, Light light, const ShadowMap& shadow_map){
		glUseProgram(shaderProgram);

		Uniform{"Projection"} = Projection;
		Uniform{"View"} = View;

		Uniform{"light_position"} = light.position;
		Uniform{"Ia"} = light.Ia;
		Uniform{"Id"} = light.Id;
		Uniform{"Is"} = light.Is;
		
		Uniform{"LightSpaceMatrix"} = shadow_map.lightSpaceMatrix;
		
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, shadow_map.depthMap);
		Uniform{"shadow_map"} = 5;
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

	GLFWwindow* window = glfwCreateWindow(800, 600, "CG UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	
	glewInit();
	glEnable(GL_DEPTH_TEST);
	enable_debug();

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    PhongShaderShadow shader;
    Scene scene;
    ShadowMap shadow_map;
    Light light;

	light.position = vec4{0, 15, 15, 1};
	light.Ia = vec3{ 0.2, 0.2, 0.2};
	light.Id = vec3{ 1.0, 1.0, 1.0};
	light.Is = vec3{ 1.0, 1.0, 1.0};
	
	float vangle = 0;
    mat4 BaseView = lookAt({0, 7, 20}, {0, 7, 0}, {0, 1, 0});

    double last_x, last_y;
    glfwGetCursorPos(window, &last_x, &last_y);
	
	while(!glfwWindowShouldClose(window)){
        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

        // Verifica mouse
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
		double dy = ypos - last_y;
		last_x = xpos;
		last_y = ypos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		    vangle += 0.01*dy;
		    vangle = clamp(vangle, -1.5, 1.5);
		    BaseView = rotate_y(dx*0.01)*BaseView;
        }

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
        BaseView = translate(xmove, 0, zmove)*BaseView;

	    // Primeira passada: renderiza o buffer de sombra
	    shadow_map.setup(light.position);
	    scene.draw();

	    // Segunda passada: renderiza o buffer de cor 
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screen_width, screen_height);

        glClearColor(1, 1, 1, 1);	
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(45, a, 0.1, 500);
        mat4 View = rotate_x(vangle)*BaseView;

        shader.setup(Projection, View, light, shadow_map);
        scene.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
