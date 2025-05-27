#pragma once

#include "Render3D.h"

class ImageZBuffer{
	ImageRGB& img;
	Image<float> zbuf;

	public:
	ImageZBuffer(ImageRGB& img): img{img}, zbuf{img.width(), img.height()}{
		zbuf.fill(1.0f);
	}

	int width() const { return img.width(); }
	int height() const { return img.height(); }
	RGB& operator()(int x, int y){ return img(x,y); }

	template<class Varying>
	friend bool testPixel(Pixel p, Varying v, ImageZBuffer& zbuffer){
		if(!testPixel(p, v, zbuffer.img))
			return false;
	
		vec4 pos = v.position;
		float z = pos[2]/pos[3];
		float& zb = zbuffer.zbuf(p.x, p.y);
		if(z < zb){
			zb = z;
			return true;
		}
		return false;
	}
};