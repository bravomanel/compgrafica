#include "Render3D.h"
#include "TextureShader.h"
#include "transforms.h"

struct Vertex{
	vec3 position;
	vec2 texCoords;
};

int main(){
	Vertex V[4] = {
		{{0, 0, 0}, {0, 0}},
		{{10, 0, 0}, {1, 0}},
		{{10, 10, 0}, {1, 1}},
		{{0, 10, 0}, {0, 1}}
	};

	TriangleFan T{4};

	int w = 600, h = 600;
	mat4 View = lookAt({8, 8, 8}, {5/3., 6, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(50, a, 0.1, 50);
	mat4 Model = rotate_y(1.9);

	TextureShader shader;
	shader.texture = {ImageRGB{"minichecker.png"}, NEAREST, CLAMP, CLAMP};
	shader.M = Projection*View*Model;

	ImageRGB G{w, h};
	G.fill(white);
	Render3D(V, T, shader, G);
	G.savePNG("output.png");
}
