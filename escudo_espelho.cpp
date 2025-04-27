#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"

int main(){
    std::vector<vec2> CP = loadCurve("tarefa07escudo.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 50);

    ImageRGB I(1000, 600);
    I.fill(white);

    vec2 center = {0.3, 0.5};

    for (int i = 0; i < 5; i++) {
        vec2 pos = {center[0] + 0.15*i, center[1]};
        mat3 T = translate(-pos);
        mat3 Ti = translate(pos);
        mat3 S = scale(0.1, 0.1);

        mat3 M = Ti * S * T;
        render2d(M*P, LineStrip{P.size()}, RGB{0, 100 + i*30, 200}, I);

        // Reflexo
        mat3 Rflip = scale(1, -1);
        mat3 M_reflex = Ti * Rflip * S * T;
        render2d(M_reflex*P, LineStrip{P.size()}, RGB{200, 100 + i*30, 0}, I);
    }

    I.save("escudos_espelho.png");
}
