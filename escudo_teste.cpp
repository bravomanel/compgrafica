#include "Render2D_v1.h"
#include "bezier.h"
#include "matrix.h"
#include "transforms2d.h"

int main(){
    std::vector<vec2> CP = loadCurve("tarefa07escudo.txt");
    std::vector<vec2> P = sample_bezier_spline<3>(CP, 50);

    ImageRGB I(1000, 700);
    I.fill(white);

    for (int i = 0; i < 15; i++) {
        float t = i / 14.0;
        float x = 0.2 + 0.6 * t;
        float y = 0.5 + 0.3*sin(2*M_PI*t);
        vec2 pos = {x, y};

        mat3 T = translate(-pos);
        mat3 Ti = translate(pos);
        mat3 R = rotate_2d(sin(2*M_PI*t) * M_PI/8);
        mat3 S = scale(0.08, 0.08);

        mat3 M = Ti * R * S * T;

        RGB color = RGB{uint8_t(100 + 10*i), uint8_t(150), uint8_t(50 + 10*i)};
        render2d(M*P, LineStrip{P.size()}, color, I);
    }

    I.save("escudos_s_caminho.png");
}
