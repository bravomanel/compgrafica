#include "Render3D.h"
#include "ZBuffer.h"
#include "ObjMesh.h"
#include "TextureShader.h"
#include "transforms.h"

int main(){
	ObjMesh mesh{"modelos/teapot.obj"};
	auto mesh_triangles = mesh.getTriangles();
	Triangles T{mesh_triangles.size()};

	TextureShader shader;
	shader.texture.img = ImageRGB{"bob.jpg"};
	shader.texture.filter = BILINEAR;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;
	
	int w = 800, h = 600;
	ImageRGB G{w, h};
	
	mat4 View = lookAt({30, 30, 30}, {0, 5, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 100);

	G.fill(white);
	ImageZBuffer I{G};
		
	float theta = 1.4*M_PI;
	mat4 Model = rotate_y(theta)*rotate_x(-M_PI/2);

	shader.M = Projection*View*Model;

	Render3D(mesh_triangles, T, shader, I);
		
	G.savePNG("output.png");
}
