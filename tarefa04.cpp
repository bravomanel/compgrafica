#include<iostream>
#include"Image.h"
void over(RGB& Bg,RGBA C)
{
	vec4 vc=toVec(C);
	float alpha=vc[3];
	vec3 col=lerp(alpha,toVec(Bg),toVec3(vc));
	Bg=toColor(col);
}
/*void composicao(RGB bg,RGBA obj,int x, int y)
{
	for(int j=0;j<obj.height();j++)
		for(int i=0;i<obj.width();i++)
			over(bg(x+i,y+j),obj(i,j));
}*/
int main()
{
	ImageRGB imagem{"BG.png"};
	ImageRGBA base1{"1.png"};
	//composicao(imagem,base1,128,0);
	int x=128,y=0;
	for(int j=0;j<base1.height();j++)
		for(int i=0;i<base1.width();i++)
			over(imagem(x+i,y+j),base1(i,j));
	ImageRGBA base2{"2.png"};
	x=256,y=0;
	for(int j=0;j<base2.height();j++)
		for(int i=0;i<base2.width();i++)
			over(imagem(x+i,y+j),base2(i,j));
	x=384,y=0;
	for(int j=0;j<base2.height();j++)
		for(int i=0;i<base2.width();i++)
			over(imagem(x+i,y+j),base2(i,j));
	x=512,y=0;
	ImageRGBA base3{"3.png"};
	for(int j=0;j<base3.height();j++)
		for(int i=0;i<base3.width();i++)
			over(imagem(x+i,y+j),base3(i,j));
	ImageRGBA boneco{"SnowMan.png"};
	x=400,y=128;
	for(int j=0;j<boneco.height();j++)
		for(int i=0;i<boneco.width();i++)
			over(imagem(x+i,y+j),boneco(i,j));
	ImageRGBA johnny{"johnny-bravo.png"};
	x=70,y=84;
	for(int j=0;j<johnny.height();j++)
		for(int i=0;i<johnny.width();i++)
			over(imagem(x+i,y+j),johnny(i,j));
	imagem.save("out.png");
}
