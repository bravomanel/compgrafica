#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "Color.h"
#include "utilsGL.h"
#include "ObjMesh.h"

using Vertex = ObjMesh::Vertex;

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

class GLMesh{
	VAO vao;
	GLBuffer vbo;
	std::vector<MaterialRange> materials;
	std::map<std::string, GLTexture> texture_map;
	public:
	mat4 Model;

	GLMesh() = default;

	GLMesh(std::string obj_file, mat4 _Model, std::string default_texture = ""){
		ObjMesh mesh{obj_file};
		init_buffers(mesh.getTriangles());

		MaterialInfo std_mat;
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);

		for(MaterialRange range: materials)
			load_texture(mesh.path, range.mat.map_Kd);
		
		Model = _Model;
	}

	void draw() const{
		glBindVertexArray(vao);
		for(MaterialRange range: materials){
			Uniform{"has_texture"} = get_texture(range.mat.map_Kd);
			Uniform{"default_color"} = range.mat.Kd;
			glDrawArrays(GL_TRIANGLES, range.first, range.count);
		}
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

			
	void load_texture(std::string path, std::string file){
		if(file != "" && texture_map.find(file) == texture_map.end()){
			std::string img = path + file;
			std::cout << "read image " << img << '\n';
			texture_map[file] = init_texture(img);
		}
	}
	
	bool get_texture(std::string file) const{
		auto it = texture_map.find(file);
		if(it != texture_map.end()){
			glBindTexture(GL_TEXTURE_2D, it->second);
			return true;
		}

		return false;
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
	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	glEnable(GL_DEPTH_TEST);

	ShaderProgram shaderProgram{
		Shader{"TextureShader.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};
	glUseProgram(shaderProgram);
	
    std::vector<GLMesh> meshes;

	meshes.emplace_back(
		"modelos/Retroville Street/bg_jimmy_06_hi.obj",
		translate(0, 1, -5)*rotate_y(0.4)*scale(1,1,1));

	meshes.emplace_back(
		"modelos/Luigi/luigi.obj",
		translate(-3.5, 1, 2.5)*rotate_y(1.4)*scale(0.01, 0.01, 0.01));

	meshes.emplace_back(
		"modelos/Mario/mario.obj",
		translate(-2, 1, 1)*rotate_y(0.7)*scale(0.01, 0.01, 0.01));

	meshes.emplace_back(
		"modelos/Arm Cannon/Light Beam.obj", 
		translate(0.4, 2.65, 1.15)*rotate_y(0.2)*scale(1,1,1));

	meshes.emplace_back(
		"modelos/Samus (PED Suit)/Samus (PED Suit + Hazard Shield, 75% Corruption).obj", 
		translate(1, 1, 1)*rotate_y(0.2)*scale(0.6,0.6,0.6));

	meshes.emplace_back(
		"modelos/Kawasaki_Ki-61/14082_WWII_Plane_Japan_Kawasaki_Ki-61_v1_L2.obj", 
		translate(0, 2, -4)*rotate_x(-1.76)*rotate_z(-1.57)*scale(5, 5, 5));

    float vangle = 0;
    mat4 BaseView = lookAt({-1, 3, 6}, {-1, 3, 0}, {0, 1, 0});

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
	    float ymove = 0;
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            zmove += 0.05;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            zmove -= 0.05;
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            xmove += 0.05;
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            xmove -= 0.05;
        if(glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
            ymove += 0.05;
        if(glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
            ymove -= 0.05;
            
        BaseView = translate(xmove, ymove, zmove)*BaseView;

        glClearColor(0, 165.0/255, 220.0/255, 1);	
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float a = screen_width/(float)screen_height;
        mat4 Projection = perspective(45, a, 0.1, 200);
        mat4 View = rotate_x(vangle)*BaseView;

        for(const GLMesh& mesh: meshes){
            mat4 Model = mesh.Model;
            Uniform{"M"} = Projection*View*Model;
            mesh.draw();
        }
    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}
