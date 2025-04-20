#include <iostream>
#include "Image.h"

void over(RGB &Bg, RGBA C)
{
	vec4 vc = toVec(C);
	float alpha = vc[3];
	vec3 col = lerp(alpha, toVec(Bg), toVec3(vc));
	Bg = toColor(col);
}

void composeImage(ImageRGB &background, const ImageRGBA &foreground, int x, int y)
{
	for (int j = 0; j < foreground.height(); ++j)
	{
		for (int i = 0; i < foreground.width(); ++i)
		{
			if (x + i >= 0 && x + i < background.width() && y + j >= 0 && y + j < background.height())
			{
				over(background(x + i, y + j), foreground(i, j));
			}
		}
	}
}

int main()
{
	ImageRGB imagem{"BG.png"};

	ImageRGBA base1{"1.png"};
	composeImage(imagem, base1, 128, 0);

	ImageRGBA base2{"2.png"};
	composeImage(imagem, base2, 256, 0);
	composeImage(imagem, base2, 384, 0);

	ImageRGBA base3{"3.png"};
	composeImage(imagem, base3, 512, 0);

	ImageRGBA boneco{"SnowMan.png"};
	composeImage(imagem, boneco, 400, 128);

	ImageRGBA johnny{"johnny-bravo.png"};
	composeImage(imagem, johnny, 70, 84);

	ImageRGBA bush{"DeadBush.png"};
	composeImage(imagem, bush, 115, 128);

	ImageRGBA tile1{"Tile1.png"};
	composeImage(imagem, tile1, 110, 420);

	ImageRGBA tile2{"Tile2.png"};
	composeImage(imagem, tile2, 238, 420);

	ImageRGBA tile3{"Tile3.png"};
	composeImage(imagem, tile3, 366, 420);

	ImageRGBA zombie{"Zombie.png"};
	composeImage(imagem, zombie, 280, 540);

	imagem.save("out.png");
}