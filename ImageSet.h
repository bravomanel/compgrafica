#pragma once

#include "Image.h"
#include <string>
#include <map>

class ImageSet{
	std::map<std::string, ImageRGB> texture_map;
	public:

	void load_texture(std::string path, std::string file){			
		if(file != "" && texture_map.find(file) == texture_map.end()){
			std::string img = path + file;
			std::cout << "read image " << img << '\n';
			texture_map[file] = ImageRGB{img};
		}
	}

	void get_texture(std::string file, ImageRGB& img) const{
		auto it = texture_map.find(file);
		img = (it != texture_map.end())? it->second: ImageRGB{};
	}
};
