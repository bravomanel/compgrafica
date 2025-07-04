#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
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
	GLuint begin = 0;
	GLuint end = 0;
	GLuint mapKd_index = 0;
	GLint children[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
};

#define MAX_VERTS 90
#define MAX_CHILDREN 8

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

	// Vetor de vértices
	std::vector<Vertex> vertices;

	// Caixa envolvente
	BoundingBox BB;

	// Porções da malha
	std::vector<Range> ranges;

	// Caixas envolventes de cada range.
	std::vector<BoundingBox> bbs;

	// Índice da raiz da hierarquia.
	int root_index;

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

		// Cria e preenche buffer de vértices.
		vertices.reserve(tris.size());
		for(auto& T: tris)
			vertices.push_back(Vertex{T.position, T.normal, T.texCoords});
		
		// Caixa envolvente global da malha.
		BB = get_bounding_box(0, vertices.size());
		
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

		bbs.reserve(mat_ranges.size());

		// Para cada porção da malha
		for(auto material : mat_ranges){
			// Calcula caixa envolvente dos vértices dentro desta porção.
			int ibegin = material.first;
			int iend = material.first + material.count;
			bbs.push_back(get_bounding_box(ibegin, iend));

			GLuint image_index = get_image_index(mesh.path + material.mat.map_Kd);
			ranges.push_back(Range{material.first, material.first+material.count, image_index});
		}

		int n_leaves = ranges.size();

		create_hierarchy(0, n_leaves);
		root_index = ranges.size() - 1;

		split_ranges(0, n_leaves);
		
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

	void create_hierarchy(int begin, int end){
		int n = end - begin;
		if(n < 2)
			return;
		
		int nbegin = ranges.size();
		int nn = ceil(n/(float)MAX_CHILDREN);
		int k = begin;
		for(int i = 0; i < nn; i++){
			int k0 = k;
			Range new_node{0, 0, 0};
			for(int j = 0; j < MAX_CHILDREN && k < end; j++){
				new_node.children[j] = k++;
			}
			int k1 = k;
			ranges.push_back(new_node);
			bbs.push_back(join_bbs(k0, k1));
		}

		create_hierarchy(nbegin, nbegin + nn);
	}

	void split_ranges(int begin_range, int end_range, int level=0){
		if(level >= 3 || begin_range >= end_range)
			return;

		int new_ranges_begin = ranges.size();
		for(int i = begin_range; i != end_range; i++){
			Range& R = ranges[i];
			int n_tris = (R.end - R.begin)/3;
			if(n_tris > 100)
				split_range(R, bbs[i]);
		}
		int new_ranges_end = ranges.size();
		split_ranges(new_ranges_begin, new_ranges_end, level+1);
	}

	void split_range(Range& R, BoundingBox& BB){
		vec3 p_mid = 0.5*(BB.pmin + BB.pmax);
		auto b = (Vertex (*)[3])&vertices[R.begin];
		auto e = (Vertex (*)[3])&vertices[R.end];
		auto Fx = [&](Vertex T[3]){ 
			return T[0].position[0] < p_mid[0] && 
			       T[1].position[0] < p_mid[0] && 
				   T[2].position[0] < p_mid[0];
		};
		auto Fy = [&](Vertex T[3]){ 
			return T[0].position[1] < p_mid[1] && 
			       T[1].position[1] < p_mid[1] && 
				   T[2].position[1] < p_mid[1];
		};
		auto Fz = [&](Vertex T[3]){ 
			return T[0].position[2] < p_mid[2] && 
			       T[1].position[2] < p_mid[2] && 
				   T[2].position[2] < p_mid[2];
		};

		auto cx  = std::partition(b, e, Fx);

		auto cy0 = std::partition( b, cx, Fy);
		auto cy1 = std::partition(cx,  e, Fy);

		auto cz0 = std::partition(  b, cy0, Fz);
		auto cz1 = std::partition(cy0,  cx, Fz);
		auto cz2 = std::partition( cx, cy1, Fz);
		auto cz3 = std::partition(cy1,   e, Fz);

		int new_ranges_begin = ranges.size();

		for(int i = 0; i < 8; i++)
			R.children[i] = i + new_ranges_begin;

		auto add_child = [&](auto a0, auto a1){
			Range new_range;
			new_range.begin = R.begin + (a0 - b)*3;
			new_range.end   = R.begin + (a1 - b)*3;
			new_range.mapKd_index = R.mapKd_index;
			ranges.push_back(new_range);

			BoundingBox new_bb = get_bounding_box(new_range.begin, new_range.end);
			bbs.push_back(new_bb);
		};

		add_child(b, cz0);
		add_child(cz0, cy0);
		add_child(cy0, cz1);
		add_child(cz1, cx);
		add_child(cx, cz2);
		add_child(cz2, cy1);
		add_child(cy1, cz3);
		add_child(cz3, e);
	}

	BoundingBox get_bounding_box(int ibegin, int iend){
		BoundingBox BB;
		BB.pmin = BB.pmax = vertices[ibegin].position;
		for(int i = ibegin+1; i < iend; i++){
			BB.pmin = min(BB.pmin, vertices[i].position);
			BB.pmax = max(BB.pmax, vertices[i].position);
		}
		return BB;
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

	BoundingBox join_bbs(int k0, int k1){
		BoundingBox bb = bbs[k0];
		for(int k = k0+1; k < k1; k++){
			bb.pmin = min(bb.pmin, bbs[k].pmin);
			bb.pmax = max(bb.pmax, bbs[k].pmax);
		}
		return bb;
	}

	void draw(){
		glViewport(0, 0, screen_width, screen_height);

		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = (float)screen_width/(float)screen_height;

		mat4 Model = R*scale(.400, .400, .400);
		mat4 View = lookAt({0, 0, 10}, 0.5*toVec3(Model*toVec4(BB.pmin+BB.pmax, 1)), {0, 1, 0});
		mat4 Projection = perspective(40, aspect, .5, 50);
		
		glUseProgram(shaderProgram);
		Uniform{"Model"} = Model;
		Uniform{"View"} = View;
		Uniform{"Projection"} = Projection;

		Uniform{"iModel"} = inverse(Model);
		Uniform{"iView"} = inverse(View);

		Uniform{"lightPos"} = vec4{10, 5, 15, 1};

		Uniform{"root_index"} = root_index;

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
