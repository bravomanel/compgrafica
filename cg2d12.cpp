#include "Render2D.h"
#include "matrix.h"
#include "transforms2d.h"

struct Vec2Col{
    vec2 position;
    RGB color;
 };
 
 
struct Varying{
    vec2 position;
    vec3 color;
};
 

int main(){
    Vec2Col P[] = {
        { { 0.0,  0.0}, red    },
        { { 0.6,  0.2}, green  },
        { {-0.4,  0.6}, blue   },
        { {-0.8, -0.4}, yellow },
        { { 0.4, -0.8}, cyan   },
        { { 0.8, -0.2}, orange },
    };
    
    auto vs = [&](int i){ // Função lambda
        return Varying{P[i].position, toVec(P[i].color)};
    };
    auto fs = [&](Varying v, RGB& color){
        color = toColor(v.color);
    };  
    
    LineStrip L{6};
    //Triangles L{6};
    
    ImageRGB I(400, 400);
    I.fill(white);
    render2d(6, L, vs, fs, I);
    I.save("output.png");
    
}