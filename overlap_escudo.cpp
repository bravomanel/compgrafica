#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"

int main(){
    std::vector<vec2> CP = loadCurve("tarefa07escudocentralizado.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 50);

    ImageRGB I(1200, 900);
    I.fill(white);

    mat3 Win = window(-4.0/3.0, 4.0/3.0, -1, 1);

    std::vector<vec2> positions = {
        {-1, 0.6}, {0, 0.6}, {1, 0.6}, {-1, -0.6}, {0, -0.6}, {1, -0.6}
    };

    for (auto pos : positions) {
        mat3 Ti = translate(pos);
        mat3 T = translate(-pos);

        for (int i = 0; i < 8; i++) {
            float s = 0.5 * pow(0.7, i);
            mat3 S = scale(s, s);
            mat3 M = Win * Ti * S * T;
            RGB color = RGB{uint8_t(255 - i*30), uint8_t(0 + i*30), uint8_t(0)};
            render2d(M*P, LineStrip{P.size()}, color, I);
        }
    }

    I.save("escudos_dentro.png");
}
