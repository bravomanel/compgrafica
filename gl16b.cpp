#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "Color.h"
#include "utilsGL.h"
#include "ObjMesh.h"

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

MaterialInfo standard_material(std::string mat_Kd){
	MaterialInfo mat;

	mat.name = "standard";

	mat.Ka = {1, 1, 1};
	mat.Kd = {1, 1, 1};
	mat.Ks = {0, 0, 0};
	mat.Ns = 1;

	mat.map_Kd = mat_Kd;

	return mat;
}

using Vertex = ObjMesh::Vertex;

struct SurfaceMesh{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

class GLMesh{
	VAO vao;
	GLBuffer vbo;
	GLBuffer vbo_tangents;
	std::vector<MaterialRange> materials;
	std::map<std::string, GLTexture> texture_map;
	public:
	mat4 Model;

	GLMesh() = default;

	GLMesh(std::string obj_file, mat4 _Model, MaterialInfo std_mat = standard_material("")){
		Model = _Model;

		ObjMesh mesh{obj_file};
		std::vector<Vertex> tris = mesh.getTriangles();
		init_buffers(tris);
		calc_tangents(tris);

		materials = mesh.getMaterials(std_mat);
		for(MaterialRange range: materials){
			load_texture(mesh.path, range.mat.map_Ka);
			load_texture(mesh.path, range.mat.map_Kd);
			load_texture(mesh.path, range.mat.map_Ks);
			load_texture(mesh.path, range.mat.map_Bump);
		}
	}
	
	void draw() const{
		for(MaterialRange range: materials)
			draw(range);
	}

	private:

	void init_buffers(const std::vector<Vertex>& vertices){
		vao = VAO{true};
		glBindVertexArray(vao);

		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(vertices);

		size_t stride = sizeof(Vertex);
		size_t offset_position = offsetof(Vertex, position);
		size_t offset_texCoords = offsetof(Vertex, texCoords);
		size_t offset_normal = offsetof(Vertex, normal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,(void*)offset_texCoords);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)offset_normal);
	}
	
	void calc_tangents(const std::vector<Vertex>& vertices){
		std::vector<vec3> tangents;
		int N = vertices.size();
		for(int i = 0; i+2<N; i+=3){
			Vertex v0 = vertices[i];
			Vertex v1 = vertices[i+1];
			Vertex v2 = vertices[i+2];
			tangents.push_back(get_tangent(v0, v1, v2));
			tangents.push_back(get_tangent(v1, v2, v0));
			tangents.push_back(get_tangent(v2, v0, v1));
		}
		
		vbo_tangents = GLBuffer{GL_ARRAY_BUFFER};
		vbo_tangents.data(tangents);
		
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	vec3 get_tangent(Vertex v0, Vertex v1, Vertex v2){
		vec3 d1 = v1.position - v0.position;
		vec3 d2 = v2.position - v0.position;

		float du1 = v1.texCoords[0] - v0.texCoords[0];
		float du2 = v2.texCoords[0] - v0.texCoords[0];
		float dv1 = v1.texCoords[1] - v0.texCoords[1];
		float dv2 = v2.texCoords[1] - v0.texCoords[1];

		mat2 A = {
			du1, dv1,
			du2, dv2
		};
		float det = determinant(A);
		// primeira linha da inversa de A
		float a = (1/det)*A(1,1);
		float b = (1/det)*(-A(0, 1));

		return a*d1 + b*d2;
	}

			
	void load_texture(std::string path, std::string file){
		if(file != "" && texture_map.find(file) == texture_map.end()){
			std::cout << "read image " << path+file << '\n';
			texture_map[file] = init_texture(path+file);
		}
	}
	
	void select_map(std::string map_name, std::string map_id, int texture_unit) const{
		auto it = texture_map.find(map_id);
		bool has_map = it != texture_map.end();
		Uniform{"has_" + map_name} = has_map;
		if(has_map){
			Uniform{map_name} = texture_unit;
			glActiveTexture(GL_TEXTURE0 + texture_unit);
			glBindTexture(GL_TEXTURE_2D, it->second);
		}
	}

	void draw(MaterialRange range) const{
		Uniform{"Ka"} = range.mat.Ka; 
		Uniform{"Kd"} = range.mat.Kd;
		Uniform{"Ks"} = range.mat.Ks;
		Uniform{"shininess"} = range.mat.Ns;

		select_map("map_Ka", range.mat.map_Ka, 0);
		select_map("map_Kd", range.mat.map_Kd, 1);
		select_map("map_Ks", range.mat.map_Ks, 2);
		select_map("normal_map", range.mat.map_Bump, 3);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, range.first, range.count);
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
	
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	ShaderProgram shaderProgram{
		Shader{"PhongShaderTexNormal.vert", GL_VERTEX_SHADER},
		Shader{"PhongShaderTexNormal.frag", GL_FRAGMENT_SHADER}
	};	
	glUseProgram(shaderProgram);

	//GLMesh mesh{"modelos/pose/pose.obj", scale(0.3, 0.3, 0.3)};
	GLMesh mesh{"modelos/train-toy-cartoon/train-toy-cartoon.obj", translate(0, 10, 0)*scale(1500, 1500, 1500)};

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

		float a = screen_width/(float)screen_height;
		mat4 Projection = perspective(45, a, 1, 500);
		mat4 View = lookAt({50, 50, 50}, {0, 30, 0}, {0, 1, 0});
		mat4 Model = rotate_y(angle)*mesh.Model;

		glUseProgram(shaderProgram);
		
		Uniform{"light_position"} = vec4{ 0, 50, 50, 0.0 };
		Uniform{"Ia"} =  vec3{ 0.8, 0.8, 0.8};
		Uniform{"Id"} =  vec3{ 1, 1, 1};
		Uniform{"Is"} = vec3{ 1, 1, 1};

		Uniform{"Projection"} = Projection;
		Uniform{"View"} = View;
		Uniform{"Model"} = Model;

		mesh.draw();
		
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}



#if 0
ShaderProgram shaderProgram;
GLMesh mesh;
unsigned int screen_width = 600;
unsigned int screen_height = 600;

float angle = 0;
vec3 mesh_center;

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"PhongShaderTexNormal.vert", GL_VERTEX_SHADER},
		Shader{"PhongShaderTexNormal.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	init_shader();
	mesh = GLMesh{"modelos/pose/pose.obj", scale(0.3, 0.3, 0.3)};
	//mesh = GLMesh{"modelos/train-toy-cartoon/train-toy-cartoon.obj", translate(0, 10, 0)*scale(1500, 1500, 1500)};
}

void desenha(){
	glClearColor(1, 1, 1, 1);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float a = screen_width/(float)screen_height;
	mat4 Projection = perspective(45, a, 1, 500);
	mat4 View = lookAt({50, 50, 50}, {0, 30, 0}, {0, 1, 0});
	mat4 Model = rotate_y(angle)*mesh.Model;

	glUseProgram(shaderProgram);
	
	Uniform{"light_position"} = vec4{ 0, 50, 50, 1.0 };
	Uniform{"Ia"} =  vec3{ 0.4, 0.4, 0.4};
	Uniform{"Id"} =  vec3{ 1, 1, 1};
	Uniform{"Is"} = vec3{ 1, 1, 1};


	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;
	Uniform{"Model"} = Model;

	mesh.draw();
}

double last_x;

void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		last_x = x;
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double /*y*/){
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS){
		double dx = x - last_x;
		angle += dx*0.01;
		last_x = x;
	}
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height){
    glViewport(0, 0, width, height);
	screen_width = width;
	screen_height = height;
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

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "CG UFF", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	

	init();

	glViewport(0, 0, screen_width, screen_height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	while(!glfwWindowShouldClose(window)){
		desenha();
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

#endif