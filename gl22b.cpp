#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "transforms.h"

struct Volumetric{
	// Variaveis do shader
	ShaderProgram shaderProgram;
	ShaderProgram backface_shaderProgram;
	ShaderProgram quad_shaderProgram;

	// Dimensões da tela
	int screen_width;
	int screen_height;
		
	// Framebuffer e textura para guardar backface 
	unsigned int backface_FBO;
	int BACKFACE_WIDTH = 1024, BACKFACE_HEIGHT = 1024;
	GLTexture backface_texture;

	// Vertex Array Object do Cubo
	VAO cube_vao;
	GLBuffer cube_vbo;
	GLBuffer cube_ebo;

	// Dados volumetricos
	GLTexture volume_texture;

	// Dados da funcao de transferencia
	GLTexture tf_texture;

	// Vertex Array Object do quad
	VAO quad_vao;
	GLBuffer quad_vbo;
	GLBuffer quad_ebo;

	// Parâmetros da renderização
	float vmin = 0.5;
	float vmax = 0.7;
	float stepSize = 0.001f;
	mat4 R = loadIdentity();

	////////////////////////////////////////////////////////////////////////////////
	// FUNCOES DE INICIALIZACAO
	Volumetric(){		
		initShaders();

		initCube();
		
		initBackfaceBuffer();

		initVolume();

		initTransferFunction();

		initQuad();
	}

	void initShaders(){
		shaderProgram = ShaderProgram{
			Shader{"volume_data/raycasting.vert", GL_VERTEX_SHADER}, 
			Shader{"volume_data/raycasting1.frag", GL_FRAGMENT_SHADER}
		};

		backface_shaderProgram = ShaderProgram{
			Shader{"volume_data/backface.vert", GL_VERTEX_SHADER}, 
			Shader{"volume_data/backface.frag", GL_FRAGMENT_SHADER}
		};

		quad_shaderProgram = ShaderProgram{
		Shader{"volume_data/quad.vert", GL_VERTEX_SHADER}, 
		Shader{"volume_data/quad.frag", GL_FRAGMENT_SHADER}
	};
	}

	void initCube(){
		// Vertices 
		vec3 P[] = {
			{0, 0, 0},	{1, 0, 0},	{1, 1, 0},	{0, 1, 0},
			{0, 0, 1},	{1, 0, 1},	{1, 1, 1},	{0, 1, 1},
		};
		unsigned int indices[] = {
			0, 2, 1,	0, 3, 2, // back
			4, 5, 7,	5, 6, 7, // front
			5, 1, 6,	1, 2, 6, // right
			0, 4, 3,	4, 7, 3, // left
			0, 1, 4,	1, 5, 4, // down
			2, 3, 6,	3, 7, 6  // up
		};
		
		cube_vao = VAO{true};
		glBindVertexArray(cube_vao);

		cube_vbo = GLBuffer{GL_ARRAY_BUFFER};

		cube_vbo.data(P);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		cube_ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
		cube_ebo.data(indices);
	}

	void initBackfaceBuffer(){
		backface_texture = GLTexture{GL_TEXTURE_2D};
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
			BACKFACE_WIDTH, BACKFACE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

		unsigned int depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 
			BACKFACE_WIDTH, BACKFACE_HEIGHT);

		glGenFramebuffers(1, &backface_FBO);  
		glBindFramebuffer(GL_FRAMEBUFFER, backface_FBO);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, backface_texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initVolume(){
		size_t w = 256, h = 256, d = 225;
		size_t size = w*h*d;
		std::vector<char> volume(size); 

		std::ifstream datafile("volume_data/head256.raw");
		if(!datafile){
			std::cerr << "could not open file\n";
			exit(1);
		}
		datafile.read(volume.data(), size);
		if(!datafile){
			std::cerr << "could not read file\n";
			exit(1);
		}
		
		volume_texture = GLTexture{GL_TEXTURE_3D};
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, volume.data());
	}

	void initTransferFunction(){
		std::ifstream inFile("volume_data/tff.dat");

		std::vector<char> tf;
		char c;
		while(inFile.get(c))
			tf.push_back(c);

		tf_texture = GLTexture{GL_TEXTURE_1D};
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, tf.size()/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, tf.data());
	}

	void initQuad(){
		float dy = 0.1;
		vec3 P[] = {	{-1, -1, -1},	{1, -1, -1},	{1, -1+dy, -1},	{-1, -1+dy, -1}	};
		
		quad_vao = VAO{true};
		glBindVertexArray(quad_vao);

		quad_vbo = GLBuffer{GL_ARRAY_BUFFER};
		quad_vbo.data(P);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	////////////////////////////////////////////////////////////////////////////////
	// FUNCOES DE DESENHO
	void draw(){
		// Desenha backface
		glBindFramebuffer(GL_FRAMEBUFFER, backface_FBO);
		glViewport(0, 0, BACKFACE_WIDTH, BACKFACE_HEIGHT);

		glUseProgram(backface_shaderProgram);
		renderCube(GL_FRONT);

		// Desenha cena
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, screen_width, screen_height);

		glUseProgram(shaderProgram);
		setup_uniforms();

		renderCube(GL_BACK);

		draw_quad();
	}

	void renderCube(GLenum cull_face){
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float aspect = screen_width/(float)screen_height;
		mat4 Projection = scale(1, 1, -1)*perspective(60, aspect, .1, 500);
		mat4 View = translate(0, 0, -1.5);
		mat4 Model = R*translate(-0.5, -0.5, -0.5);
		Uniform{"MVP"} = Projection*View*Model;

		glCullFace(cull_face);
		glBindVertexArray(cube_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	void setup_uniforms(){
		Uniform{"ScreenSize"} = vec2{(float)screen_width, (float)screen_height};
		Uniform{"StepSize"} = stepSize;

		Uniform{"ExitPoints"} = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, backface_texture);

		Uniform{"VolumeTex"} = 1;
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, volume_texture);
		
		Uniform{"TransferFunction"} = 2;
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_1D, tf_texture);
	}

	void draw_quad(){
		glUseProgram(quad_shaderProgram);

		Uniform{"TransferFunction"} = 2;
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_1D, tf_texture);

		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
	enable_debug();	
	
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Volumetric volume;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;
	double last_y = ypos;
	while(!glfwWindowShouldClose(window)){
		// Verifica mouse
		glfwGetCursorPos(window, &xpos, &ypos);
		double dx = xpos - last_x;
		double dy = ypos - last_y;
        last_x = xpos;
		last_y = ypos;
	    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		    volume.R = rotate_y(dx*0.01)*rotate_x(dy*0.01)*volume.R;

		// Atualiza tamanho da tela
        glfwGetWindowSize(window, &volume.screen_width, &volume.screen_height);

		// Desenha volume
		volume.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

