#include "Render3D.h"
#include "ZBuffer.h"
#include "TextureShader.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "ImageSet.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
		"modelos/Retroville Street/bg_jimmy_06_hi.obj",
		translate(0, 1, -5)*rotate_y(0.4)*scale(1,1,1));

		
	meshes.emplace_back(
		"modelos/Kawasaki_Ki-61/14082_WWII_Plane_Japan_Kawasaki_Ki-61_v1_L2.obj", 
		translate(0, 2, -4)*rotate_x(-1.76)*rotate_z(-1.57)*scale(5, 5, 5));
		
	meshes.emplace_back(
		"modelos/Samus (PED Suit)/Samus (PED Suit + Hazard Shield, 75% Corruption).obj", 
		translate(1, 1, 1)*rotate_y(0.2)*scale(0.6,0.6,0.6));
		
	meshes.emplace_back(
		"modelos/Arm Cannon/Light Beam.obj", 
		translate(0.4, 2.65, 1.15)*rotate_y(0.2)*scale(1,1,1));

	meshes.emplace_back(
		"modelos/Luigi/luigi.obj",
		translate(-3.5, 1, 2.5)*rotate_y(1.4)*scale(0.01, 0.01, 0.01));

	meshes.emplace_back(
		"modelos/Mario/mario.obj",
		translate(-2, 1, 1)*rotate_y(0.7)*scale(0.01, 0.01, 0.01));


	TextureShader shader;
	shader.texture.filter = NEAREST;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;

	int w = 800, h = 600;
	float a = w/(float)h;
	mat4 Projection = perspective(45, a, 0.1, 1000);

	vec3 p0 = {4, 2, 12};
	vec3 p1 = {4, 2, -12};

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
