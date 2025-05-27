#include "Render3D.h"

struct Shader{
    struct Varying{
        vec4 position;
    };

    void vertexShader(vec2 v, Varying& out){
	    out.position = toVec4(v);
	};

    void fragmentShader(Varying V, RGB& fragColor){
        fragColor = blue;
    }
};

int main(){
    Shader shader;

	vec2 V[] = {
		{-0.8, -0.6}, {-0.6,  0.8}, {-0.2, -0.6},
		{ 0.0,  0.0}, {0.6,  0.8},	{ 0.8, -0.8}
	};

    Triangles T{6};

    ImageRGB G{200, 200};
    G.fill(white);
    Render3D(V, T, shader, G);
	G.savePNG("output.png");
}
