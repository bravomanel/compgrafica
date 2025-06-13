#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Color.h"
#include "utilsGL.h"
#include "transforms.h"

struct Vertex{
	vec3 position;
	vec3 normal;
};

class Surface{
    VAO vao;
    GLBuffer vbo;
    GLBuffer ebo;
    int n_verts;

    public:
    Surface(int m, int n){
        int N = m*n;

        float u0 = -5, u1 = 5, du = (u1-u0)/(m-1),
            v0 = -5, v1 = 5, dv = (v1-v0)/(n-1);

        std::vector<Vertex> V(N);
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                float u = u0 + i*du;
                float v = v0 + j*dv;

                int ij = i + j*m;
                V[ij].position = {u, v, sinf(u*v/4)};

                vec3 du = {1, 0, cosf(u*v/4)*v/4};
                vec3 dv = {0, 1, cosf(u*v/4)*u/4};
                V[ij].normal = normalize(cross(du, dv));
            }
        }

        std::vector<unsigned int> indices;
        for(int i = 0; i < m-1; i++){
            for(int j = 0; j < n-1; j++){	
                unsigned int ij = i + j*m;
                indices.insert(indices.end(), {
                        ij, ij+1, ij+m,
                        ij+m+1, ij+m, ij+1
                        });
            }
        }
        n_verts = indices.size();
	
        vao = VAO{true};
        glBindVertexArray(vao);

        vbo = GLBuffer{GL_ARRAY_BUFFER};
        vbo.data(V);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        
        ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
        ebo.data(indices);
    }

    void draw(){
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, n_verts, GL_UNSIGNED_INT, 0);
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
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
	};

    Surface surface{50, 50};

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

        glUseProgram(shaderProgram);
        Uniform{"Model"} = rotate_z(angle);
        Uniform{"View"} = lookAt({10, 10, 10}, {0, 0, 0}, {0, 0, 1});
        float a = screen_width/(float)screen_height;
        Uniform{"Projection"} = perspective(50, a, 0.1, 50);

        Uniform{"Ka"}  = vec3{0.7, 0.7, 1.0};
        Uniform{"Kd"}  = vec3{0.7, 0.7, 1.0};
        Uniform{"Ks"}  = vec3{1.0, 1.0, 1.0};
        Uniform{"shininess"} = 140.0f;

        Uniform{"light_position"} = vec4{ 0.0, 8.0, 10.0, 1.0 };
        Uniform{"Ia"}  = vec3{ 0.2, 0.2, 0.2};
        Uniform{"Id"}  = vec3{ 1.0, 1.0, 1.0};
        Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

        surface.draw();

        glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

