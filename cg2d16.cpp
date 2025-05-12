#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"

int main(){
	std::vector<vec2> P = {{-0.2, -0.3}, {4, -0.3}, {4, 0.3}, {-0.2, 0.3}};
	TriangleFan T{4};

	ImageRGB I(800, 600);

	mat3 W = window(-2, 10, -1, 8);
	int N = 60;
	for(int i = 0; i < N; i++){
		I.fill(0xf5f5f5_rgb);
		float theta = i/(N-1.0);
		mat3 A = rotate_2d(theta);
		mat3 B = A*translate(3.8, 0)*rotate_2d(2*theta)*scale(0.8, 0.8);
		mat3 C = B*translate(3.8, 0)*rotate_2d(2.5*theta)*scale(0.7, 0.7);
		render2d(W*A*P, T, red, I);
		render2d(W*B*P, T, blue, I);
		render2d(W*C*P, T, green, I);
		I.save_frame(i, "anim/output", "png");
	}
	
	I.savePNG("output.png");
}
