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
		std_mat.name = "standard material";
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);
		std::cout << "materials size: " << materials.size() << '\n';

		for(MaterialRange range: materials)
			image_set.load_texture(mesh.path, range.mat.map_Kd);

		Model = _Model;
	}

	void draw(ImageZBuffer& G, TextureShader& shader) const{
		for(const MaterialRange& range: materials){
			shader.texture.default_color = toColor(range.mat.Kd);
			image_set.get_texture(range.mat.map_Kd, shader.texture.img);
			TrianglesRange T{range.first, range.count};
			Render3D(tris, T, shader, G);
		}
	}
};

int main(){
	std::vector<Mesh> meshes;

	meshes.emplace_back(
		"modelos/floor.obj", 
		scale(35, 35, 35), 
		"../brickwall.jpg");

	meshes.emplace_back(
		"modelos/box.obj", 
		translate(1, 1, -6)*rotate_y(-0.4),
		"../bob.jpg");

	meshes.emplace_back(
		"modelos/metroid/DolBarriersuit.obj", 
		translate(-1, 0, -1)*rotate_y(0.2)*scale(.1, .1, .1));

	meshes.emplace_back(
		"modelos/pose/pose.obj", 
		translate(1, 0, 0)*scale(0.009, 0.009, 0.009));

	meshes.emplace_back(
		"modelos/House Complex/House Complex.obj", 
		translate(4, 0, 0)*rotate_y(0.5*M_PI)*scale(.15, .15, .15));

	meshes.emplace_back(
		"modelos/pony-cartoon/Pony_cartoon.obj", 
		translate(-2, 0, -3)*scale(0.005, 0.005, 0.005)
	);

	TextureShader shader;
	shader.texture.filter = NEAREST;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;

	int w = 800, h = 600;
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 1000);

	vec3 p0 = {2, 1.7, 10};
	vec3 p1 = {-1, 2.7, -18};

	ImageRGB G{w, h};
	int nframes = 60;
	for(int k = 0; k < nframes; k++){
		G.fill(0x00A5DC_rgb);
		ImageZBuffer I{G};
	
		float t = k/(nframes-1.0);
		vec3 pos = lerp(t, p0, p1);
		mat4 View = lookAt(pos, vec3{0, 1.5, 0}, {0, 1, 0});

		for(const Mesh& mesh: meshes){
			shader.M = Projection*View*mesh.Model;
			mesh.draw(I, shader);
		}

		G.save_frame(k, "anim/output", "bmp");
	}
}