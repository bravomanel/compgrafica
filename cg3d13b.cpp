#include "Render3D.h"
#include "ZBuffer.h"
#include "TextureShader.h"
#include "ObjMesh.h"
#include "transforms.h"

int main(){
	std::string path = "modelos/train-toy-cartoon/";
	ObjMesh mesh{path+"train-toy-cartoon.obj"};
	auto mesh_triangles = mesh.getTriangles();

	int w = 600, h = 600;
	ImageRGB G{w, h};
	G.fill(white);
	ImageZBuffer I{G};

	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.01, 100);
	mat4 View = lookAt({.03, .03, .03}, mesh.center(), {0, 1, 0});
	mat4 Model = rotate_y(.2);

	TextureShader shader;
	shader.M = Projection*View*Model;
	shader.texture.filter = BILINEAR;
	
	for(auto range: mesh.getMaterials()){
		shader.texture.img = ImageRGB{path + range.mat.map_Kd};
		TrianglesRange T{range.first, range.count};
		Render3D(mesh_triangles, T, shader, I);
	}

	G.savePNG("output.png");
}
