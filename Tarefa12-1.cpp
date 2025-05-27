#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main(){
	std::vector<vec2> P={{-4,0},{4,0},{4,40},{-4,40}};

	TriangleFan T{4};

	ImageRGB I(600,600);

	mat3 W=window(-100,100,-100,100);
	int N=60;
	for(int i=0;i<N;i++){
		I.fill(0xf5f5f5_rgb);
		float theta=i/(N-1.0);
		mat3 Corpo=rotate_2d(0.2*theta);
		mat3 Cabeca=Corpo*translate(0,40)*rotate_2d(0.2*theta)*scale(1.2,0.3);
		mat3 BracoL1=Corpo*translate(-4,36)*rotate_2d(1*theta+1.57)*scale(0.6,0.6);
		mat3 BracoR1=Corpo*translate(4,36)*rotate_2d(-1*theta-1.57)*scale(0.6,0.6);
		mat3 BracoL2=BracoL1*translate(0,40)*rotate_2d(-0.4*theta)*scale(0.8,0.8);
		mat3 BracoR2=BracoR1*translate(0,40)*rotate_2d(-0.4*theta)*scale(0.8,0.8);
		mat3 MaoL=BracoL2*translate(0,40)*rotate_2d(-1*theta)*scale(1.4,0.4);
		mat3 MaoR=BracoR2*translate(0,40)*rotate_2d(-1*theta)*scale(1.4,0.4);
		mat3 PernaL1=Corpo*translate(-4,0)*rotate_2d(-0.6*theta+3.14)*scale(0.8,0.8);
		mat3 PernaR1=Corpo*translate(4,0)*rotate_2d(0.6*theta+3.14)*scale(0.8,0.8);
		mat3 PernaL2=PernaL1*translate(0,40)*rotate_2d(0.8*theta)*scale(0.8,0.8);
		mat3 PernaR2=PernaR1*translate(0,40)*rotate_2d(0.8*theta)*scale(0.8,0.8);
		mat3 PeL=PernaL2*translate(4,38)*rotate_2d(-0.4*theta-1.57)*scale(0.6,0.4);
		mat3 PeR=PernaR2*translate(4,38)*rotate_2d(-0.4*theta-1.57)*scale(0.6,0.4);
		
		render2d(W*Corpo*P,T,red,I);
		render2d(W*Cabeca*P,T,blue,I);
		render2d(W*BracoL1*P,T,green,I);
		render2d(W*BracoR1*P,T,green,I);
		render2d(W*BracoL2*P,T,red,I);
		render2d(W*BracoR2*P,T,red,I);
		render2d(W*MaoL*P,T,blue,I);
		render2d(W*MaoR*P,T,blue,I);
		render2d(W*PernaL1*P,T,green,I);
		render2d(W*PernaR1*P,T,green,I);
		render2d(W*PernaL2*P,T,red,I);
		render2d(W*PernaR2*P,T,red,I);
		render2d(W*PeL*P,T,blue,I);
		render2d(W*PeR*P,T,blue,I);
		I.save_frame(i,"anim/output","png");
	}
	
	I.savePNG("output.png");
}
