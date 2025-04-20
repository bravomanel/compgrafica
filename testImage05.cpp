#include "Image.h"

float G(float x, float y, float cx, float cy, float s){
	float dx = x - cx;
	float dy = y - cy;
	return 100*exp(-(dx*dx + dy*dy)/(s*s));
}

float F(float x, float y){
	return 50 
	// Torax
	- G(x, y, 450, 600, 130) //Central
		// Linha superior 	
	- G(x, y, 350, 750, 40)
	- G(x, y, 400, 750, 40)
	- G(x, y, 450, 750, 40)
	- G(x, y, 500, 750, 40)
	- G(x, y, 550, 750, 40)
		// Linha direita
	- G(x, y, 550, 700, 30)
	- G(x, y, 550, 650, 30)
	- G(x, y, 550, 600, 30)
	- G(x, y, 550, 550, 30)
	- G(x, y, 550, 500, 30)
		// Linha esquerda
	- G(x, y, 350, 700, 30)
	- G(x, y, 350, 650, 30)
	- G(x, y, 350, 600, 30)
	- G(x, y, 350, 550, 30)
	- G(x, y, 350, 500, 30)

	//Perna Esquerda
	- G(x, y, 390, 420, 50) 
	- G(x, y, 350, 420, 40) 
	- G(x, y, 350, 400, 40) 
	- G(x, y, 340, 350, 50) 
	- G(x, y, 330, 300, 30) 
	- G(x, y, 320, 250, 40) 
	- G(x, y, 310, 200, 30) 
	- G(x, y, 300, 150, 30) 
	- G(x, y, 290, 100, 30) 
		// Pé
	- G(x, y, 260, 100, 30) 
	- G(x, y, 230, 100, 30) 
	- G(x, y, 200, 100, 30) 

	//Perna Direita
	- G(x, y, 510, 420, 50)
	- G(x, y, 550, 420, 40)
	- G(x, y, 550, 400, 40)
	- G(x, y, 560, 350, 50)
	- G(x, y, 570, 300, 30)
	- G(x, y, 580, 250, 40)
	- G(x, y, 590, 200, 30)
	- G(x, y, 600, 150, 30)
	- G(x, y, 610, 100, 30)
		// Pé
	- G(x, y, 640, 100, 30)
	- G(x, y, 670, 100, 30)
	- G(x, y, 700, 100, 30)

	// Braco Esquerdo
	- G(x, y, 290, 750, 45)
	- G(x, y, 270, 700, 28)
	- G(x, y, 260, 700, 28)
	- G(x, y, 250, 700, 28)
	- G(x, y, 240, 690, 28)
	- G(x, y, 230, 670, 28)
	- G(x, y, 220, 650, 28)
	- G(x, y, 210, 630, 28)
	- G(x, y, 200, 610, 28)
	- G(x, y, 200, 590, 28)
	- G(x, y, 200, 570, 28)
	- G(x, y, 200, 550, 20)
	 	// Mao
	- G(x, y, 200, 530, 15)
	- G(x, y, 200, 510, 30)
	- G(x, y, 200, 500, 30)

	// Braco Direito
	- G(x, y, 610, 750, 45)
	- G(x, y, 630, 700, 28)
	- G(x, y, 640, 700, 28)
	- G(x, y, 650, 700, 28)
	- G(x, y, 660, 690, 28)
	- G(x, y, 670, 670, 28)
	- G(x, y, 680, 650, 28)
	- G(x, y, 690, 630, 28)
	- G(x, y, 700, 610, 28)
	- G(x, y, 700, 590, 28)
	- G(x, y, 700, 570, 28)
	- G(x, y, 700, 550, 20)
	 	// Mao
	- G(x, y, 700, 530, 15)
	- G(x, y, 700, 510, 30)
	- G(x, y, 700, 500, 30)


	
	- G(x, y, 450, 800, 30) //Pescoco
	 
		//Cabeca
	- G(x, y, 450, 990, 73) 
	- G(x, y, 450, 900, 73); 
}
	
int main(){
	ImageRGB img(900, 1200);
	img.fill(white);
	for(int y = 0; y < img.height(); y++)
		for(int x = 0; x < img.width(); x++)
			if(F(x,y) <= 0)
				img(x,y) = red;
		

	img.save("output.png");
}
