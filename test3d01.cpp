#include "Render2D.h"
#include "transforms2d.h"
#include "transforms.h"

int main(){
	// Coordenadas do modelo
	std::vector<vec4> V = {
		{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, 
		{-1, 1, -1, 1},{-1, -1, 1, 1}, {1, -1, 1, 1}, 
		{1, 1, 1, 1}, {-1, 1, 1, 1}, {0, 1.6, -1, 1}, {0, 1.6, 1, 1}
	};
	unsigned int indices[] = {
		0, 1,	1, 2,	2, 3,	3, 0,	4, 5,	5, 6,
		6, 7,	7, 4,	0, 4,	1, 5,	2, 6,	3, 7,
		9, 7,   9, 6,   3, 8,   8, 2,   9, 8
	};
	Elements<Lines> L{indices};
	
	// Resolução da imagem.
	int w = 600, h = 600;

	// Janela de visualização
	float l = -2, r = 2;
	float b = -1.8, t = 2.2;
	mat3 Window = window(l, r, b, t);

#define PROJECTION_TYPE 1

#if PROJECTION_TYPE==1 // Projeção ortogonal
	mat3x4 Proj = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	};

	mat4 View = loadIdentity(); // projeção no plano xy
	//mat4 View = lookAt({1, 0, 0}, {0, 0, 0}, {0, 1, 0}); // projeção no plano zy
	//mat4 View = lookAt({0, 1, 0}, {0, 0, 0}, {0, 0, 1}); // projeção no plano xz
	//mat4 View = lookAt({1, 1, 1}, {0, 0, 0}, {0, 1, 0}); // isométrica
	//mat4 View = lookAt({1, .2, 1}, {0, 0, 0}, {0, 1, 0}); // dimétrica
	//mat4 View = lookAt({.3, .2, 1}, {0, 0, 0}, {0, 1, 0}); // trimétrica
	
////////////////////////////////////////////////////////////
#elif PROJECTION_TYPE==2 // Projeção oblíqua
	mat3x4 Proj = {
		1, 0, 0.3, 0,
		0, 1, 0.4, 0,
		0, 0, 0.0, 1
	};

	mat4 View = loadIdentity();

////////////////////////////////////////////////////////////
#else // Projeção perspectiva
	// Plano de projeção z = n
	float n = 4;
	mat3x4 Proj = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1/n, 0
	};
	mat4 View = lookAt({2, 1.4, 4}, {0, 0, 0}, {0, 1, 0});
#endif
	
	// Matriz do modelo.
	mat4 Model = loadIdentity();

	// Composição das transformações.
	mat3x4 M = Window*Proj*View*Model;
	
	ImageRGB G(w, h);
	G.fill(white);
	render2d(M*V, L, red, G);
	G.savePNG("output.png");
}
