#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Color.h"
#include "utilsGL.h"
#include "VertexUtils.h"
#include "MixColorShader.h"
#include "MarchingCubes.h"
#include "transforms.h"
#include "auto_diff.h"

template<class T>
T sphere(T x, T y, T z){
	float r = 1;
	return x*x + y*y + z*z - r*r;
}

template<class T>
T torus(T x, T y, T z){
	float R = 1.5;
	float r = 0.4;
	return pow(sqrt(x*x + y*y) - R, 2) + z*z - r*r;
}

template<class T>
T treco(T x, T y, T z){
	T r = (x*x + y*y);
	return 2*y*(y*y - 3*x*x)*(1-z*z) + r*r - (9*z*z - 1)*(1 - z*z);
}
    

template<class T>
T outro_treco(T x, T y, T z){
    return  sqrt( (pow(x-1,2) + y*y + z*z)*
                  (pow(x+1,2) + y*y + z*z)*
                  (x*x + pow(y-1,2) + z*z)*
                  (x*x + pow(y+1,2) + z*z) 
            ) - 1.1;
}

template<class F>
vec3 get_normal(vec3 p, F f){
   Dual<3> x = {p[0], 1, 0, 0};
   Dual<3> y = {p[1], 0, 1, 0};
   Dual<3> z = {p[2], 0, 0, 1};
   Dual<3> a = f(x,y,z);
   return a.df;
}

class Surface{
    VAO vao;
    GLBuffer vbo_position;
    GLBuffer vbo_normal;
    int n_verts;
    public:
    Surface(int m, int n, int p){
        vec3 pmin = {-2, -2, -2}; 
        vec3 pmax = { 2,  2,  2};

        std::vector<vec3> Position = marchingCubes(sphere<float>, m, n, p, pmin, pmax);

        n_verts = Position.size();

        std::vector<vec3> Normal(n_verts);
        for(int i = 0; i < n_verts; i++)
            Normal[i] = get_normal(Position[i], sphere<Dual<3>>);

        vao = VAO{true};
        glBindVertexArray(vao);

        vbo_position = GLBuffer{GL_ARRAY_BUFFER};
        vbo_position.data(Position);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        
        vbo_normal = GLBuffer{GL_ARRAY_BUFFER};
        vbo_normal.data(Normal);
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    void draw(){
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, n_verts);
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

   	ShaderProgram shaderProgram {
		Shader{"PhongShader.vert", GL_VERTEX_SHADER},
		Shader{"PhongShader.frag", GL_FRAGMENT_SHADER}
	};
    Surface surface{50, 50, 50};

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
        Uniform{"View"} = lookAt({3, 3, 1.5}, {0, 0, 0}, {0, 0, 1});
        float a = screen_width/(float)screen_height;
        Uniform{"Projection"} = perspective(50, a, 0.1, 50);

        Uniform{"Ka"}   = vec3{0.7, 0.7, 1.0};
        Uniform{"Kd"}   = vec3{0.7, 0.7, 1.0};
        Uniform{"Ks"}  = vec3{ 1.0, 1.0, 1.0};
        Uniform{"shininess"} = 40.0f;

        Uniform{"light_position"} = vec4{ 0.0, 8.0, 10.0, 0.0 };
        Uniform{"Ia"}  = vec3{ 0.2, 0.2, 0.2};
        Uniform{"Id"}  = vec3{ 1.0, 1.0, 1.0};
        Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        surface.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

