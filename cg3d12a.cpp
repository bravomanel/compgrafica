#include "Render3D.h"
#include "TextureShader.h"
#include "transforms.h"

struct Vertex{
	vec3 position;
	vec2 texCoords;
};

int main(){
	Vertex V[4] = {
		{{0, 0, 0}, {-1, -1}},
		{{10, 0, 0}, {3, -1}},
		{{10, 10, 0}, {3, 3}},
		{{0, 10, 0}, {-1, 3}}
	};

	TriangleFan T{4};

	int w = 600, h = 600;
	mat4 View = lookAt({5, 5, 15}, {5, 5, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(50, a, 0.1, 50);
	mat4 Model = rotate_y(0);

	TextureShader shader;
	//shader.texture.img = ImageRGB{"minichecker.png"};
	shader.texture.img = ImageRGB{"bob.jpg"};
	shader.texture.filter = NEAREST;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;
	shader.M = Projection*View*Model;

	ImageRGB G{w, h};
	G.fill(white);
	Render3D(V, T, shader, G);
	G.savePNG("output.png");
}
