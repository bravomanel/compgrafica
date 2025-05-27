#include "Render3D.h"
#include "ZBuffer.h"
#include "TextureShader.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "ImageSet.h"

class Mesh{
	std::vector<ObjMesh::Vertex> tris;
	std::vector<MaterialRange> materials;
	ImageSet image_set;
	public:
	mat4 Model;

	Mesh(std::string obj_file, mat4 _Model, std::string default_texture = ""){
		ObjMesh mesh{obj_file};
		tris = mesh.getTriangles();

		MaterialInfo std_mat;
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);

		for(MaterialRange range: materials)
			image_set.load_texture(mesh.path, range.mat.map_Kd);

		Model = _Model;
	}
	
	void draw(ImageZBuffer& G, TextureShader& shader) const{
		for(MaterialRange range: materials){
			image_set.get_texture(range.mat.map_Kd, shader.texture.img);
			TrianglesRange T{range.first, range.count};
			Render3D(tris, T, shader, G);
		}
	}
};

int main(){
	Mesh mesh{
		"modelos/train-toy-cartoon/train-toy-cartoon.obj", translate(0, -4, 0)*scale(300, 300, 300)
		//"modelos/pose/pose.obj", translate(0, -5, 0)*scale(0.05, 0.05, 0.05)
		//"modelos/Wood Table/Old Wood Table.obj", loadIdentity()
		//"modelos/metroid/DolBarriersuit.obj", translate(0, -5.5, 0)*scale(0.6, 0.6, 0.6)
	};

	int w = 800, h = 600;
	ImageRGB G{w, h};
	
	TextureShader shader;
	shader.texture.filter = BILINEAR;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;
	
	mat4 View = lookAt({10, 5, 10}, {0, 0, 0}, {0, 1, 0});
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 100);

	int nframes = 30;
	for(int k = 0; k < nframes; k++){
		G.fill(white);
		ImageZBuffer I{G};
		
		float theta = k*2*M_PI/(nframes-1);
		mat4 Model = rotate_y(theta)*mesh.Model;
		shader.M = Projection*View*Model;

		mesh.draw(I, shader);
		
		G.save_frame(k, "anim/output", "png");
	}
}
