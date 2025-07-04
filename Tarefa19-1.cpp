#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Headers que você já possui no seu projeto
#include "utilsGL.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"

// Headers padrão do C++
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream> 
#include <cstdio>  

//================================================================================
// CÓDIGO DOS SHADERS
//================================================================================
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat3 u_transform;
uniform vec3 u_color;

out vec3 f_color;

void main()
{
    vec3 transformed_pos = u_transform * vec3(aPos, 1.0);
    gl_Position = vec4(transformed_pos.xy, 0.0, 1.0);
    f_color = u_color;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 f_color;

void main()
{
    FragColor = vec4(f_color, 1.0);
}
)glsl";


//================================================================================
// FUNÇÕES DE CALLBACK E VARIÁVEIS GLOBAIS
//================================================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
mat3 window_transform(int screen_width, int screen_height);

int g_numLayers = 8;
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

//================================================================================
// FUNÇÃO PRINCIPAL
//================================================================================
int main() {
    // --- 1. Inicialização do GLFW ---
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tarefa 19-1", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // --- 2. Inicialização do GLEW ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar o GLEW" << std::endl;
        return -1;
    }

    // --- 3. CRIAR, USAR E DESTRUIR ARQUIVOS DE SHADER TEMPORÁRIOS ---
    const char* vert_path = "_temp_shader.vert";
    const char* frag_path = "_temp_shader.frag";

    std::ofstream vert_file(vert_path);
    vert_file << vertexShaderSource;
    vert_file.close();

    std::ofstream frag_file(frag_path);
    frag_file << fragmentShaderSource;
    frag_file.close();

    Shader vertexShader(vert_path, GL_VERTEX_SHADER);
    Shader fragmentShader(frag_path, GL_FRAGMENT_SHADER);
    ShaderProgram shaderProgram({vertexShader, fragmentShader});

    remove(vert_path);
    remove(frag_path);


    // --- 4. Carregar Geometria da Curva ---
    std::vector<vec2> CP = loadCurve("tarefa07escudocentralizado.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 50);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, P.size() * sizeof(vec2), P.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 5. Loop de Renderização ---
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram.id);

        int transformLoc = glGetUniformLocation(shaderProgram.id, "u_transform");
        int colorLoc = glGetUniformLocation(shaderProgram.id, "u_color");

        mat3 Win = window_transform(SCR_WIDTH, SCR_HEIGHT);
        std::vector<vec2> positions = {
            {-1, 0.6}, {0, 0.6}, {1, 0.6}, {-1, -0.6}, {0, -0.6}, {1, -0.6}
        };

        for (auto pos : positions) {
            // CORREÇÃO 1: Acessar elementos do vec2 com um único índice.
            mat3 Ti = translate(pos[0], pos[1]);
            for (int i = 0; i < g_numLayers; i++) {
                float s = 0.5 * pow(0.7, i);
                mat3 S = scale(s, s);
                mat3 M = Win * Ti * S;

                float r = (255 - i * 30) / 255.0f;
                float g = (0 + i * 30) / 255.0f;
                glUniform3f(colorLoc, r, g, 0.0f);

                // CORREÇÃO 2: Obter ponteiro para os dados da mat3 com um único índice.
                glUniformMatrix3fv(transformLoc, 1, GL_FALSE, &M[0]);

                glBindVertexArray(VAO);
                glDrawArrays(GL_LINE_STRIP, 0, P.size());
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 6. Limpeza ---
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram.id);
    glfwTerminate();
    return 0;
}

//================================================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES
//================================================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {
            g_numLayers++;
            std::cout << "Camadas: " << g_numLayers << std::endl;
        }
        if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
            if (g_numLayers > 1) {
                g_numLayers--;
                std::cout << "Camadas: " << g_numLayers << std::endl;
            }
        }
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

mat3 window_transform(int screen_width, int screen_height) {
    float aspect = (float)screen_width / (float)screen_height;
    return scale(1.0f / aspect, 1.0f);
}