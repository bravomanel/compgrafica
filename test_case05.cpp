#include "acutest.h"
#include "transforms.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool close_matrices(mat4 A, mat4 B){
    return norm2(A - B) < 1e-5;
}

void test_rotate_n(){
    vec3 n = {1, 2, 3};

    mat4 R = rotate(n, 0.0);
    mat4 Expected = Id<4>();

    TEST_CHECK(close_matrices(R, Expected));

    R = rotate(n, 0.4);
    Expected = {
         0.926699, -0.300952,  0.225068,  0,
         0.323506,  0.943615, -0.0702454, 0,
        -0.191237,  0.137907,  0.971807,  0,
         0,         0,         0,         1
    };

    TEST_CHECK(close_matrices(R, Expected));
}

void test_rotate_x(){
    vec3 e1 = {1, 0, 0};
    int n = 1000;
    for(int i = 0; i < n; i++){
        float angle = i*2*M_PI/n;
        mat4 R = rotate(e1, angle);
        mat4 Expected = rotate_x(angle);

        TEST_CHECK(close_matrices(R, Expected));
    }
}

void test_rotate_y(){
    vec3 e2 = {0, 1, 0};
    int n = 1000;
    for(int i = 0; i < n; i++){
        float angle = i*2*M_PI/n;
        mat4 R = rotate(e2, angle);
        mat4 Expected = rotate_y(angle);

        TEST_CHECK(close_matrices(R, Expected));
    }
}

void test_rotate_z(){
    vec3 e3 = {0, 0, 1};
    int n = 1000;
    for(int i = 0; i < n; i++){
        float angle = i*2*M_PI/n;
        mat4 R = rotate(e3, angle);
        mat4 Expected = rotate_z(angle);

        TEST_CHECK(close_matrices(R, Expected));
    }
}


TEST_LIST = {
    {"rotate n", test_rotate_n},
    {"rotate x", test_rotate_x},
    {"rotate y", test_rotate_y},
    {"rotate z", test_rotate_z},
    {NULL, NULL}
};