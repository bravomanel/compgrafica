#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "VertexUtils.h"
#include "MarchingCubes.h"
#include "transforms.h"

//================================================================================
// Definição das Metaballs
// Esta seção foi copiada do seu código original da Tarefa 15.
//================================================================================

// Define uma única metaball com sua função de influência.
struct Metaball {
    float a; // Amplitude
    float b; // Raio de influência
    vec3 center; // Posição central

    // A função de campo escalar para a metaball.
    // Retorna a "força" da metaball em um ponto P no espaço.
    float operator()(vec3 P) {
        float r = norm(P - center);

        if (r >= b)
            return 0.0f;
        else if (r >= b / 3.0f)
            return 3.0f * a / 2.0f * pow(1.0f - r / b, 2.0f);
        else
            return a * (1.0f - 3.0f * r * r / (b * b));
    }
};

// Agrupa múltiplas metaballs para formar uma única superfície implícita.
struct MetaballFigure {
    float T; // Limiar (Threshold) para a isosuperfície
    std::vector<Metaball> metaballs;

    // A função de campo escalar combinada para todas as metaballs.
    // O Marching Cubes encontrará a superfície onde esta função é igual a zero.
    float operator()(float x, float y, float z) {
        vec3 P = {x, y, z};

        float sum = 0;
        for (Metaball& fi : metaballs)
            sum += fi(P);

        // A superfície é definida por sum(P) = T, ou seja, T - sum(P) = 0.
        return T - sum;
    }
};


//================================================================================
// Função Principal com Renderização OpenGL
//================================================================================

int main() {
    //----------------------------------------------------------------------------
    // 1. Inicialização do GLFW e GLEW (padrão para OpenGL)
    //----------------------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Habilita anti-aliasing (MSAA)
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tarefa 19 - Metaballs com OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    // Habilita o teste de profundidade para que os triângulos mais próximos
    // ocultem os que estão mais distantes.
    glEnable(GL_DEPTH_TEST);

    //----------------------------------------------------------------------------
    // 2. Criação da Figura de Metaballs e Geração da Malha
    //----------------------------------------------------------------------------
    MetaballFigure figure;
    figure.T = 0.5; // Limiar da isosuperfície
    figure.metaballs = {
        {1, 1,   {-0.6, 0, 0}},
        {1, 0.9, {0.1, 0, 0}},
        {1, 0.8, {0.6, 0, 0}},
        {1, 0.4, {0.9, 0, 0}},
        {1, 0.2, {1.1, 0, 0}},
        {1, 0.3, {-0.7, 0, 0.6}},
        {1, 0.3, {-0.7, 0, -0.6}},
        {1, 0.3, {-0.7, 0.6, 0}},
        {1, 0.3, {-0.7, -0.6, 0}},
        {1, 0.2, {-0.9, 0, 0.7}},
        {1, 0.2, {-0.9, 0, -0.7}},
        {1, 0.2, {-0.9, 0.7, 0}},
        {1, 0.2, {-0.9, -0.7, 0}},
    };

    vec3 pmin = {-2, -2, -2};
    vec3 pmax = {2, 2, 2};

    // Gera a malha de triângulos (lista de vértices) usando Marching Cubes.
    std::cout << "Gerando malha com Marching Cubes..." << std::endl;
    std::vector<vec3> V = marchingCubes(figure, 50, 50, 50, pmin, pmax);
    std::cout << "Malha gerada com " << V.size() << " vertices." << std::endl;

    //----------------------------------------------------------------------------
    // 3. Configuração dos Shaders e Uniforms
    //----------------------------------------------------------------------------
    ShaderProgram shaderProgram{
        Shader{"MixColorShader.vert", GL_VERTEX_SHADER},
        Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}
    };
    glUseProgram(shaderProgram);

    // Envia dados que não mudam durante o loop para a GPU (uniforms).
    Uniform{"pmin"} = pmin;
    Uniform{"pmax"} = pmax;
    Uniform{"C"} = {
        toVec(red),   toVec(blue),    toVec(green), toVec(yellow),
        toVec(orange),toVec(magenta), toVec(cyan),  toVec(purple)
    };

    //----------------------------------------------------------------------------
    // 4. Configuração dos Buffers de Vértices (VAO e VBO)
    //----------------------------------------------------------------------------
    VAO vao{true};
    glBindVertexArray(vao);

    // Cria um buffer na GPU e envia os dados dos vértices (V) para ele.
    GLBuffer vbo{GL_ARRAY_BUFFER};
    vbo.data(V);

    // Diz ao OpenGL como interpretar os dados no buffer:
    // Atributo de vértice 0 (location=0 no shader)
    // 3 componentes (x, y, z)
    // do tipo GL_FLOAT
    // não normalizados
    // sem espaçamento extra entre vértices (stride = 0)
    // começando do início do buffer (offset = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //----------------------------------------------------------------------------
    // 5. Loop de Renderização
    //----------------------------------------------------------------------------
    float angle = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double last_x = xpos;

    while (!glfwWindowShouldClose(window)) {
        // Captura de input para rotação com o mouse
        glfwGetCursorPos(window, &xpos, &ypos);
        double dx = xpos - last_x;
        last_x = xpos;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            angle += dx * 0.01; // Ajusta a velocidade da rotação
        }

        // Obtém o tamanho da janela para ajustar a projeção e o viewport
        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

        // Limpa os buffers de cor e profundidade
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calcula as matrizes de transformação
        mat4 Model = rotate_z(angle);
        mat4 View = lookAt({3.5, 3.5, 2.0}, {0, 0, 0}, {0, 0, 1}); // Posição da câmera
        float a = screen_width / (float)screen_height;
        mat4 Projection = perspective(50, a, 0.1, 50);

        // Envia a matriz de transformação combinada para o shader
        Uniform{"M"} = Projection * View * Model;

        // Desenha os triângulos
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, V.size());

        // Troca os buffers (o que foi desenhado vai para a tela) e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //----------------------------------------------------------------------------
    // 6. Finalização
    //----------------------------------------------------------------------------
    glfwTerminate();
    return 0;
}