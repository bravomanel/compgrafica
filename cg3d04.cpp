#include "Render3D.h"
#include "ZBuffer.h"
#include "ColorShader.h"
#include "VertexUtils.h"
#include "transforms.h"

int main(){
	RGB darkred   = 0x463232_rgb;
	RGB darkgreen = 0x324632_rgb;
	RGB darkblue  = 0x323246_rgb;
	Vec3Col V1[] = {
		{{0, 0, 0}, red},
		{{1, 0, 0}, red},
		{{1, 1, 0}, red},
		{{0, 1, 0}, red},
		{{0, 0, 1}, darkred},
		{{1, 0, 1}, darkred},
		{{1, 1, 1}, darkred},
		{{0, 1, 1}, darkred},
	};
	Vec3Col V2[] = {
		{{0, 0, 0}, green},
		{{1, 0, 0}, green},
		{{1, 1, 0}, green},
		{{0, 1, 0}, green},
		{{0, 0, 1}, darkgreen},
		{{1, 0, 1}, darkgreen},
		{{1, 1, 1}, darkgreen},
		{{0, 1, 1}, darkgreen},
	};
	Vec3Col V3[] = {
		{{0, 0, 0}, blue},
		{{1, 0, 0}, blue},
		{{1, 1, 0}, blue},
		{{0, 1, 0}, blue},
		{{0, 0, 1}, darkblue},
		{{1, 0, 1}, darkblue},
		{{1, 1, 1}, darkblue},
		{{0, 1, 1}, darkblue},
	};
	
	unsigned int indices[] = {
		0, 2, 1,	0, 3, 2, // back
 		4, 5, 7,	5, 6, 7, // front
		5, 1, 6,	1, 2, 6, // right
		0, 4, 3,	4, 7, 3, // left
		0, 1, 4,	1, 5, 4, // down
		2, 3, 6,	3, 7, 6  // up
	};
	Elements<Triangles> Te{indices};

	int w = 1280, h = 720;
	ImageRGB G{w, h};
	
	// Model Matrix
	mat4 Model1 = translate(-1, 0, 0);
	mat4 Model2 = translate(1, 0, -1)*rotate_y(0.4);
	mat4 Model3 = translate(1, 0, 1)*rotate_y(-0.3)*scale(0.3, 1.8, 1);
	
	// Projection Matrix
	mat4 Projection = perspective(50, w/(float)h, .1, 10);

	int nframes = 60;
	for(int k = 0; k < nframes; ++k){
		G.fill(white);
		ImageZBuffer I{G};

		// View Matrix
		float theta = k*2*M_PI/nframes;
		vec3 pos = {4*cosf(theta), 3, 4*sinf(theta)};
		mat4 View = lookAt(pos, {0, 0, 0}, {0, 1, 0});

		ColorShader shader;
		shader.M = Projection*View*Model1;
		Render3D(V1, Te, shader, I);
		
		shader.M = Projection*View*Model2;
		Render3D(V2, Te, shader, I);

		shader.M = Projection*View*Model3;
		Render3D(V3, Te, shader, I);

		G.save_frame(k, "anim/output", "png");
	}
}
