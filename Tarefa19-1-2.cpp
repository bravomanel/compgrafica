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

// Variáveis Globais para Interatividade
int g_numLayers = 8;
vec2 g_position_offset = {0.0f, 0.0f}; // Guarda o deslocamento base do teclado

// Configurações da janela
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tarefa 19-1 | Efeito Paralaxe", NULL, NULL);
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
        
        // Posições base dos escudos
        std::vector<vec2> positions = {
            {-1, 0.6}, {0, 0.6}, {1, 0.6}, {-1, -0.6}, {0, -0.6}, {1, -0.6}
        };

        // NOVO: Multiplicadores de profundidade para cada escudo
        // Quanto maior o número, "mais longe" e mais rápido ele se move.
        std::vector<float> depth_multipliers = {
            2.0f, 1.0f, 2.0f,   // Linha de cima (central se move menos)
            4.0f, 3.0f, 4.0f    // Linha de baixo (cantos se movem mais)
        };

        // MODIFICADO: Loop com índice para acessar o multiplicador de cada escudo
        for (size_t i = 0; i < positions.size(); ++i) {
            vec2 base_pos = positions[i];
            float multiplier = depth_multipliers[i];

            // A posição final é a base + o deslocamento do teclado multiplicado pelo fator de profundidade
            float final_x = base_pos[0] + g_position_offset[0] * multiplier;
            float final_y = base_pos[1] + g_position_offset[1] * multiplier;

            mat3 Ti = translate(final_x, final_y);
            
            for (int j = 0; j < g_numLayers; ++j) {
                float s = 0.5 * pow(0.7, j);
                mat3 S = scale(s, s);
                mat3 M = Win * Ti * S;

                float r = (255 - j * 30) / 255.0f;
                float g = (0 + j * 30) / 255.0f;
                glUniform3f(colorLoc, r, g, 0.0f);
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
// IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES (sem alterações)
//================================================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float move_step = 0.05f;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) g_numLayers++;
        if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
            if (g_numLayers > 1) g_numLayers--;
        }
        if (key == GLFW_KEY_UP)    g_position_offset[1] += move_step;
        if (key == GLFW_KEY_DOWN)  g_position_offset[1] -= move_step;
        if (key == GLFW_KEY_LEFT)  g_position_offset[0] -= move_step;
        if (key == GLFW_KEY_RIGHT) g_position_offset[0] += move_step;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    }
}

mat3 window_transform(int screen_width, int screen_height) {
    float aspect = (float)screen_width / (float)screen_height;
    return scale(1.0f / aspect, 1.0f);
}