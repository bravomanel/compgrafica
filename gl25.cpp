#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <map>
#include "utilsGL.h"
#include "transforms.h"
#include "ObjMesh.h"

// Dados de um vértice com requisitos de alinhamento 
// para ser utilizado no shader.
struct Vertex{
	alignas(16) vec3 position;
	alignas(16) vec3 normal;
	alignas(16) vec2 texCoords;
};

// Porção da malha associada a um determinado material
struct Range{
	GLuint first;
	GLuint count;
	GLuint mapKd_index;
};

// Caixa envolvente.
struct BoundingBox{
	alignas(16) vec3 pmin;
	alignas(16) vec3 pmax;
};

vec3 min(vec3 u, vec3 v){
	return {std::min(u[0], v[0]), std::min(u[1], v[1]), std::min(u[2], v[2])};
}

vec3 max(vec3 u, vec3 v){
	return {std::max(u[0], v[0]), std::max(u[1], v[1]), std::max(u[2], v[2])};
}

// Sistema de traçado de raios.
struct RayTracing{
	// Resolução da imagem.
	int screen_width = 600;
	int screen_height = 600;

	// Dados de buffers e shaders
	VAO vao;
	ShaderProgram shaderProgram;
	GLTexture mapKd[32];
	GLBuffer vertices_buffer;
	GLBuffer ranges_buffer;
	GLBuffer bbs_buffer;

	// Caixa envolvente
	BoundingBox BB;

	// Porções da malha
	std::vector<Range> ranges;

	// Hashmap de caminhos para imagens para índices
	std::map<std::string, int> image_map;

	// Contagem de imagens.
	int image_count;

	// Controle da rotacao
	mat4 R = loadIdentity();

	// Construtor
	RayTracing(){
		glEnable(GL_DEPTH_TEST); 
		
		shaderProgram = ShaderProgram{
			Shader{"RayTracing.vert", GL_VERTEX_SHADER},
			Shader{"RayTracingMesh.frag", GL_FRAGMENT_SHADER}
		};

		vao = VAO{true};

		//ObjMesh mesh{"modelos/train-toy-cartoon/train-toy-cartoon.obj"};
		//ObjMesh mesh{"modelos/pose/pose.obj"};
		ObjMesh mesh{"modelos/metroid/DolBarriersuit.obj"};
		//ObjMesh mesh{"modelos/pony-cartoon/Pony_cartoon.obj"};
		auto tris = mesh.getTriangles();

		// Caixa envolvente global da malha.
		BB.pmin = BB.pmax = tris[0].position;
		for(auto V: tris){
			BB.pmin = min(BB.pmin, V.position);
			BB.pmax = max(BB.pmax, V.position);
		}

		// Cria e preenche buffer de vértices.
		std::vector<Vertex> vertices;
		vertices.reserve(tris.size());
		for(auto& T: tris)
			vertices.push_back(Vertex{T.position, T.normal, T.texCoords});

		// contador de imagens.
		image_count = 0;

		// Textura padrão branca utilizada quando não houver mapa difuso.
		image_map[""] = 0;
		glActiveTexture(GL_TEXTURE0 + image_count);
		mapKd[image_count] = GLTexture{GL_TEXTURE_2D};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &white);
		image_count++;

		// Obtém a sequência de materiais da malha.
		auto mat_ranges = mesh.getMaterials();
		ranges.reserve(mat_ranges.size());

		std::vector<BoundingBox> bbs;
		bbs.reserve(mat_ranges.size());

		// Para cada porção da malha
		for(auto material : mat_ranges){
			// Calcula caixa envolvente dos vértices dentro desta porção.
			vec3 pmin, pmax; 
			pmin = pmax = tris[material.first].position;
			for(GLuint i = material.first; i < material.first + material.count; i++){
				vec3 p = vertices[i].position;
				pmin = min(pmin, p);
				pmax = max(pmax, p);
			}
			bbs.push_back(BoundingBox{pmin, pmax});
			GLuint image_index = get_image_index(mesh.path + material.mat.map_Kd);
			ranges.push_back(Range{material.first, material.count, image_index});
		}

		vertices_buffer = GLBuffer{GL_SHADER_STORAGE_BUFFER};
		vertices_buffer.data(vertices);
    	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertices_buffer);

		ranges_buffer = GLBuffer{GL_SHADER_STORAGE_BUFFER};
		ranges_buffer.data(ranges);
    	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ranges_buffer);

		bbs_buffer = GLBuffer{GL_SHADER_STORAGE_BUFFER};
		bbs_buffer.data(bbs);
    	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bbs_buffer);
	}

	GLuint get_image_index(std::string filename){
		auto it = image_map.find(filename);
		if(it == image_map.end()){
			std::cout << "load image: " << filename << '\n';
			image_map[filename] = image_count;

			glActiveTexture(GL_TEXTURE0 + image_count);
			mapKd[image_count] = GLTexture{GL_TEXTURE_2D};
			mapKd[image_count].load(filename);
			image_count++;
				
			glGenerateMipmap(GL_TEXTURE_2D);
				
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				
			if(GLEW_EXT_texture_filter_anisotropic){
				GLfloat fLargest;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
			}
		}
		return image_map[ filename ];
	}

	void draw(){
		glViewport(0, 0, screen_width, screen_height);

		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = (float)screen_width/(float)screen_height;

		mat4 Model = R*scale(.200, .200, .200);
		mat4 View = lookAt({0, 0, 10}, 0.5*toVec3(Model*toVec4(BB.pmin+BB.pmax, 1)), {0, 1, 0});
		mat4 Projection = perspective(40, aspect, .5, 50);
		
		glUseProgram(shaderProgram);
		Uniform{"Model"} = Model;
		Uniform{"View"} = View;
		Uniform{"Projection"} = Projection;

		Uniform{"iModel"} = inverse(Model);
		Uniform{"iView"} = inverse(View);

		Uniform{"lightPos"} = vec4{10, 5, 15, 1};

		for(int i = 0; i < image_count; i++)
			Uniform{"mapKd[" + std::to_string(i) + "]"} = i ;

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void update_angles(float dx, float dy){
		R = rotate_y(0.01*dx)*rotate_x(0.01*dy)*R;
	}
};

int main(){
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

		scene.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
