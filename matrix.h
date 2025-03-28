#ifndef MATRIX_H
#define MATRIX_H

#include <cstdio>
#include <array>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>

#if __cplusplus < 201703L
#error "CONFIGURE O SEU COMPILADOR PARA UTILIZAR A VERSÃO C++17"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template<size_t N, class A, class B>
float dot(const A& a, const B& b){
    float r = 0;
    for(size_t i = 0; i < N; i++)
        r += a[i]*b[i];
    return r;
}

template<size_t size, size_t stride>
struct DataHandler{
    float* v;
    size_t offset = 0;

    float& operator[](size_t j) { return v[j*stride]; }
    
    template<class Op>
    DataHandler apply(Op op){
        for(size_t j = offset; j < size; j++)
            op(j);
        return *this;
    }
    
    template<class X>
    DataHandler operator=(const X& x){
        return apply([&](size_t j){ (*this)[j] = x[j]; });
    }

    template<class X>
    DataHandler operator+=(const X& x){
        return apply([&](size_t j){ (*this)[j] += x[j]; });
    }
    
    template<class X>
    DataHandler operator-=(const X& x){
        return apply([&](size_t j){ (*this)[j] -= x[j]; });
    }

    DataHandler operator*=(float a){
        return apply([&](size_t j){ (*this)[j] *= a; });
    }

    DataHandler operator/=(float a){
        return apply([&](size_t j){ (*this)[j] /= a; });
    }
};


template<size_t M, size_t N=M>
struct Mat{
    float v[M*N];

    using Row    = DataHandler<N, 1>;
    using Column = DataHandler<M, N>;
    using Data   = DataHandler<M*N, 1>;
    
    Row    row(size_t i, size_t j = 0)    { return {v+i*N, j}; }
    const Row    crow(size_t i, size_t j = 0) const   { return {v+i*N, j}; }
    Column column(size_t j) { return {v+j};   }
    Data   data()        { return {v};     }
    
    float& operator[](size_t j)       { return v[j]; }
    float  operator[](size_t j) const { return v[j]; }
    
    float& operator()(size_t i, size_t j)       { return v[i*N + j]; }
    float  operator()(size_t i, size_t j) const { return v[i*N + j]; }

    Mat operator+(Mat B) const{
        B.data() += *this;
        return B;
    }
    
    Mat operator-(const Mat& B) const{
        Mat R = *this;
        R.data() -= B;
        return R;
    }
    
    Mat operator-() const{
        Mat R;
        for(size_t i = 0u; i < M*N; i++)
            R.v[i] = -v[i];
        return R;
    }
    
    Mat<1, N> get_row(size_t i) const{
        Mat<1, N> R;
        const Mat& A = *this;
        for(size_t j = 0; j < N; j++)
            R[j] = A(i,j);
        return R;
    }
    
    Mat<M, 1> get_column(size_t j) const{
        const Mat& A = *this;
        Mat<M,1> C;
        for(size_t i = 0u; i < M; i++)
            C[i] = A(i,j);
        return C;
    }

    template<size_t P>
    Mat<M, P> operator*(const Mat<N, P>& B) const{
        const Mat& A = *this;
        Mat<M, P> R;
        for(size_t i = 0; i < M; i++){
            for(size_t j = 0; j < P; j++){
                R(i, j) = 0;
                for(size_t k = 0; k < N; k++)
                    R(i,j) += A(i,k)*B(k,j);
            }
        }
        return R;
    }
    
    void swap_rows(size_t i0, size_t i1){
        Mat& A = *this;
        for(size_t j = 0; j < N; j++)
            std::swap(A(i0, j), A(i1, j));
    }

    friend void print(const Mat& A){
        for(size_t i = 0; i < M; i++){
            for(size_t j = 0; j < N; j++)
                printf("%10.4f ", A(i, j));
            putchar('\n');
        }
    }

    friend void print(std::string name, const Mat& A){
        printf("%s = ", name.c_str());
        if(M > 1)
            putchar('\n');
        print(A);
    }

    friend Mat<N, M> transpose(const Mat& A){
        Mat<N, M> r;
        for(size_t j = 0; j < N; j++)
            r.row(j) = A.get_column(j);
        return r;
    }
    
    friend Mat operator*(float x, Mat A){
        A.data() *= x;
        return A;
    }
    
    friend float norm2(const Mat& A){
        return dot<M*N>(A.v, A.v);
    }
    
    friend float norm(const Mat& A){
        return sqrt(norm2(A));
    }
};

using mat2 = Mat<2>;
using mat3 = Mat<3>;
using mat4 = Mat<4>;

using mat2x2 = Mat<2,2>;
using mat2x3 = Mat<2,3>;
using mat2x4 = Mat<2,4>;

using mat3x2 = Mat<3,2>;
using mat3x3 = Mat<3,3>;
using mat3x4 = Mat<3,4>;

using mat4x2 = Mat<4,2>;
using mat4x3 = Mat<4,3>;
using mat4x4 = Mat<4,4>;

template<size_t M>    
Mat<M> Id(){
    Mat<M> R;
    for(size_t i = 0; i < M; i++)
        for(size_t j = 0; j < M; j++)
            R(i, j) = (i==j);
    return R;
}

template<size_t M>
Mat<M-1> submatrix(const Mat<M>& A, size_t i0, size_t j0){
    Mat<M-1> R;
    size_t k = 0;
    for(size_t i = 0; i < M; i++){
        if(i == i0)
            continue;

        for(size_t j = 0; j < M; j++){
            if(j == j0)
                continue;

            R.v[k] = A(i, j);
            k++;
        }
    }
    
    return R;
}

template<size_t M>
float determinant(const Mat<M>& A);

template<size_t M>
float cofactor(const Mat<M>& A, size_t i, size_t j){
    float r = determinant(submatrix(A, i, j));
    if((i+j)%2!=0)
       r = -r;
    return r;
}

inline float determinant(const Mat<1>& A){
    return A(0, 0);
}

inline float determinant(const mat2& A){
    return A(0, 0)*A(1,1) - A(1, 0)*A(0, 1);
}

inline float determinant(const mat3& A){
    return A(0, 0)*(A(1,1)*A(2,2) - A(1, 2)*A(2,1))
          -A(1, 0)*(A(0,1)*A(2,2) - A(0, 2)*A(2, 1))
          +A(2, 0)*(A(0,1)*A(1,2) - A(0, 2)*A(1,1));
}

template<size_t M>
float determinant(const Mat<M>& A){
    float det = 0;
    for(size_t i = 0; i < M; i++)
        det += A(i, 0)*cofactor(A, i, 0);
    return det;
}

template<size_t M>    
void pivot_rows(size_t k, Mat<M>& A, Mat<M>& B){
    // find row with maximum absolute value at column k
    size_t imax = k;
    for(size_t i = k+1; i < M; i++)
        if(fabs(A(i,k)) > fabs(A(imax,k)))
            imax = i;
    
    if(k == imax)
        return;

    A.swap_rows(k, imax);
    
    B.swap_rows(k, imax);
}

template<size_t M>    
void update_rows(size_t k, Mat<M>& A, Mat<M>& B){
    B.row(k) /= A(k,k);
    A.row(k,k+1) /= A(k,k);

    auto Ak = A.get_row(k);
    auto Bk = B.get_row(k);

    // for each other row: Ai = Ai - A[i][k]*Ak
    for(size_t i = 0; i < M; i++){
        if(i == k)
            continue;
    
        float s = A(i,k);
        A.row(i, k+1) -= s*Ak;
        B.row(i) -= s*Bk;
    }
}

template<size_t M>    
Mat<M> inverse(const Mat<M>& _A){
    Mat<M> A = _A;
    Mat<M> B = Id<M>();
    for(size_t k = 0; k < M; k++){
        pivot_rows(k, A, B);
        update_rows(k, A, B);
    }
    return B;
}

inline mat4 toMat4(mat3 A){
	return {
        A(0, 0), A(0, 1), A(0, 2), 0,
        A(1, 0), A(1, 1), A(1, 2), 0,
        A(2, 0), A(2, 1), A(2, 2), 0,
		      0,       0,       0, 1
	};
}

inline mat3 toMat3(mat4 A){
	return {
        A(0, 0), A(0, 1), A(0, 2), 
        A(1, 0), A(1, 1), A(1, 2), 
        A(2, 0), A(2, 1), A(2, 2)
	};
}

#endif