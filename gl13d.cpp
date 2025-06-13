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

struct Metaball{
	float a;
	float b;
	vec3 center;

	float operator()(vec3 P){
		float r = norm(P - center);

		return 
		    r >= b   ? 0.0f : 
		    r >= b/3 ? 3*a/2*pow(1 - r/b, 2): 
			           a*(1 - 3*r*r/(b*b));
	}

    vec3 normal(vec3 P){
		float r = norm(P - center);
        Dual<3> X = {P[0], 1, 0, 0};
        Dual<3> Y = {P[1], 0, 1, 0};
        Dual<3> Z = {P[2], 0, 0, 1};
        Dual<3> R = sqrt(pow(X-center[0], 2) + pow(Y-center[1], 2) + pow(Z-center[2], 2));
        Dual<3> F = 
            r >= b  ? Dual<3>{0, 0, 0, 0}:
            r >= b/3? 3*a/2*pow(1-R/b, 2):
                      a*(1 - 3*R*R/(b*b));
        return F.df;
        }
};

struct MetaballFigure{
	float T;
	std::vector<Metaball> metaballs;

	float operator()(float x, float y, float z){
		vec3 P = {x, y, z};

		float sum = 0;
		for (Metaball fi : metaballs)
			sum += fi(P);

		return T - sum;
	}

    vec3 normal(vec3 P){
        vec3 sum = {0, 0, 0};
        for(Metaball fi: metaballs)
            sum = sum + fi.normal(P);
        return -sum;
    }
};

class Surface{
    VAO vao;
    GLBuffer vbo_position;
    GLBuffer vbo_normal;
    int n_verts;
    public:
    Surface(int m, int n, int p){
        MetaballFigure figure;
        figure.T = 0.5;
        figure.metaballs = {
            {1, 1.5, { 0, 0, 0}},
            {1, 0.7, { 0, 0.1, 1}},
            {1, 0.5, { 1, 0, 0}},
            {1, 0.5, {-1, 0, 0}},
            {1, 0.5, {0.4, 0,-0.9}},
            {1, 0.5, {-0.4, 0,-0.9}},
        };

        vec3 pmin = {-2, -2, -2}; 
        vec3 pmax = { 2,  2,  2};

        std::vector<vec3> Position = marchingCubes(figure, m, n, p, pmin, pmax);

        n_verts = Position.size();

        std::vector<vec3> Normal(n_verts);
        for(int i = 0; i < n_verts; i++)
		    Normal[i] = figure.normal(Position[i]);

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

        Uniform{"Ka"}  = vec3{0.7, 0.7, 1.0};
        Uniform{"Kd"}  = vec3{0.7, 0.7, 1.0};
        Uniform{"Ks"}  = vec3{ 1.0, 1.0, 1.0};
        Uniform{"shininess"} = 40.0f;

        Uniform{"light_position"} = vec4{ 0.0, 8.0, 10.0, 0.0 };
        Uniform{"Ia"} = vec3{ 0.2, 0.2, 0.2};
        Uniform{"Id"} = vec3{ 1.0, 1.0, 1.0};
        Uniform{"Is"} = vec3{ 1.0, 1.0, 1.0};

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        surface.draw();

    	glfwSwapBuffers(window);
    	glfwPollEvents();    
	}
	glfwTerminate();
}

