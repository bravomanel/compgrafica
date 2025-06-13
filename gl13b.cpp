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

vec4 bezier_blending(float t){
    return {
        (1-t)*(1-t)*(1-t),
        3*(1-t)*(1-t)*t,
        3*(1-t)*t*t,
        t*t*t
    };
}

vec4 bezier_derivatives(float t){
    return {
        -3*(1-t)*(1-t),
        3*(1 - 3*t)*(1-t),
        3*t*(2 - 3*t),
        3*t*t
    };
}

void bezier_surface(float u, float v, vec3 P[4][4], vec3& S, vec3& N){
    vec4 U = bezier_blending(u);
    vec4 V = bezier_blending(v);
    
    vec4 dU = bezier_derivatives(u);
    vec4 dV = bezier_derivatives(v);

    mat3x4 P0 = toMat(P[0][0],  P[0][1],  P[0][2],  P[0][3]);
    mat3x4 P1 = toMat(P[1][0],  P[1][1],  P[1][2],  P[1][3]);
    mat3x4 P2 = toMat(P[2][0],  P[2][1],  P[2][2],  P[2][3]);
    mat3x4 P3 = toMat(P[3][0],  P[3][1],  P[3][2],  P[3][3]);

	mat3x4 C  = toMat(P0*U, P1*U, P2*U, P3*U);
	mat3x4 dC = toMat(P0*dU, P1*dU, P2*dU, P3*dU);

	S = C*V;
    N = normalize(cross(dC*V, C*dV));
}

class Surface{
    VAO vao;
    GLBuffer vbo_position;
    GLBuffer vbo_normal;
    GLBuffer ebo;
    int n_indices;
    public:
    Surface(int m, int n){
        int n_verts = m*n;

        vec3 P[4][4] = {
            {{-3, -3, 0}, {-1, -3, 1}, {1, -3, 2}, {3, -3, -1}},
            {{-3, -1, 1}, {-1, -1, 2}, {1, -1, 0}, {3, -1, 1}},
            {{-3,  1,-1}, {-1,  1, 1}, {1,  1, 0}, {3,  1, 0}},
            {{-3,  3, 0}, {-1,  3,-1}, {1,  3, 1}, {3,  3, 1}}
        };
        float u0 = 0, u1 = 1, du = (u1-u0)/(m-1),
            v0 = 0, v1 = 1, dv = (v1-v0)/(n-1);

        std::vector<vec3> Position(n_verts);
        std::vector<vec3> Normal(n_verts);
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                float u = u0 + i*du;
                float v = v0 + j*dv;

                unsigned int ij = i + j*m;
                bezier_surface(u, v, P, Position[ij], Normal[ij]);
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
        n_indices = indices.size();

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
        
        ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
        ebo.data(indices);
    }

    void draw(){
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
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
        Uniform{"View"} = lookAt({8, 8, 8.5}, {0, 0, 0}, {0, 0, 1});
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

