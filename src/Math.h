#pragma once

#include "Utility.h"

#include <iostream>
#include <cstring>
#include <algorithm>
#include <cassert>

#pragma region tuple

struct Point;
struct Vector;
struct Color;
struct Tuple
{
    Tuple(void);
    Tuple(float x, float y, float z, float w);

    static float Dot(const Tuple& lhs, const Tuple& rhs);

    Tuple operator+(const Tuple& rhs) const;
    Tuple operator-(const Tuple& rhs) const;
    Tuple operator-(void) const;
    Tuple operator*(float rhs) const;
    bool operator==(const Tuple& rhs) const;
    bool operator!=(const Tuple& rhs) const;
    float& operator[](int index);
    float operator[](int index) const;

    operator Point(void);
    operator Vector(void);
    operator Color(void);

    union { float x, r; };
    union { float y, g; };
    union { float z, b; };
    union { float w, a; };
};

Tuple operator*(float lhs, const Tuple& rhs);
std::ostream& operator<<(std::ostream& out, const Tuple& tuple);

#pragma endregion

#pragma region point

struct Point : public Tuple
{
    Point(void);
    Point(float x, float y, float z);
};

#pragma endregion

#pragma region vector

struct Vector : public Tuple
{
    Vector(void);
    Vector(float x, float y, float z);

    static Vector Normalize(const Vector& vec);
    static Vector Cross(const Vector& lhs, const Vector& rhs);
    static Vector Reflect(const Vector& in, const Vector& normal);

    float Magnitude(void) const;
    Vector& Normalize(void);
};

#pragma endregion

#pragma region color

struct Color : public Tuple
{
    Color(void);
    Color(float r, float g, float b, float a = 1.0f);

    Color operator*(const Color& rhs) const;
};

#pragma endregion

#pragma region matrix

template<int size = 4>
class MatrixRow
{
public:
    MatrixRow(float *data);
    MatrixRow(const float *data);

    float& operator[](int index);
    float operator[](int index) const;

private:
    union
    {
        float *m_Data;
        const float *m_ConstData;
    };
    
};

class Matrix4;
template<int size = 4>
class Matrix
{
public:
    Matrix(void);
    Matrix(const float *data);

    const float *GetData(void) const;
    
    static Matrix<size> Identity(void);
    static Matrix<size> Transpose(const Matrix<size>& m);
    static Matrix<size> Invert(const Matrix<size>& m);

    Matrix<size>& Transpose(void);
    float Determinant(void) const;
    Matrix<size - 1> Submatrix(int row, int col) const;
    float Minor(int row, int col) const;
    float Cofactor(int row, int col) const;
    Matrix<size>& Invert(void);

    Matrix<size> operator*(const Matrix<size>& rhs) const;
    MatrixRow<size> operator[](int index);
    const MatrixRow<size> operator[](int index) const;
    bool operator==(const Matrix<size>& rhs) const;
    bool operator!=(const Matrix<size>& rhs) const;

    operator Matrix4(void);

protected:
    float m_Data[size * size];
};

template<>
float Matrix<2>::Determinant(void) const;

template<int size>
std::ostream& operator<<(std::ostream& out, const Matrix<size>& rhs);

Tuple operator*(const Matrix<4>& lhs, const Tuple& rhs);

typedef Matrix<2> Matrix2;
typedef Matrix<3> Matrix3;

class Matrix4 : public Matrix<4>
{
public:
    Matrix4(void);
    Matrix4(const float *data);
    Matrix4(const Tuple& r0, const Tuple& r1, const Tuple& r2, const Tuple& r3);

    static Matrix4 Translate(float x, float y, float z);
    static Matrix4 Scale(float x, float y, float z);
    static Matrix4 RotateX(float a);
    static Matrix4 RotateY(float a);
    static Matrix4 RotateZ(float a);
    static Matrix4 Shear(float xy, float xz, float yx, float yz, float zx, float zy);
    static Matrix4 LookAt(const Vector& direction, const Vector& up);
    static Matrix4 LookAt(const Point& eye, const Point& at, const Vector& up);
    static Matrix4 Perspective(float fovY, float aspect, float zNear, float zFar);
    static Matrix4 Frustum(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far);
};

#pragma region matrix_impl

template<int size>
MatrixRow<size>::MatrixRow(float *data)
    : m_Data(data) {}

template<int size>
MatrixRow<size>::MatrixRow(const float *data)
    : m_ConstData(data) {}

template<int size>
float& MatrixRow<size>::operator[](int index)
{
    return m_Data[index];
}

template<int size>
float MatrixRow<size>::operator[](int index) const
{
    return m_ConstData[index];
}

template<int size>
Matrix<size>::Matrix(void)
{
    assert(size >= 2 && size <= 4);
    std::memset(m_Data, 0, size * size * sizeof(float));  
}

template<int size>
Matrix<size>::Matrix(const float *data)
{
    assert(size >= 2 && size <= 4);
    std::copy(data, data + size * size, m_Data);
}

template<int size>
const float *Matrix<size>::GetData(void) const
{
    return m_Data;
}

template<int size>
Matrix<size> Matrix<size>::Identity(void)
{
    Matrix<size> result;
    for(int i = 0; i < size; i++)
        result[i][i] = 1.0f;
    return result;
}

template<int size>
Matrix<size> Matrix<size>::Transpose(const Matrix<size>& m)
{
    Matrix<size> result;
    for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
            result[i][j] = m[j][i];
    return result;
}

template<int size>
Matrix<size> Matrix<size>::Invert(const Matrix<size>& m)
{
    float det = m.Determinant();
    if(Equal(det, 0.0f))
        return Matrix<size>::Identity();
    
    Matrix<size> result;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            float c = m.Cofactor(i, j);
            result[j][i] = c / det;
        }
    }

    return result;
}

template<int size>
Matrix<size>& Matrix<size>::Transpose(void)
{
    *this = Matrix<size>::Transpose(*this);
    return *this;
}

template<int size>
float Matrix<size>::Determinant(void) const
{
    float det = 0.0f;
    for(int i = 0; i < size; i++)
    {
        det += (*this)[0][i] * Cofactor(0, i);
    }
    return det;
}

template<int size>
Matrix<size - 1> Matrix<size>::Submatrix(int row, int col) const
{
    Matrix<size - 1> result;
    for(int i = 0; i < size - 1; i++) {
        for(int j = 0; j < size - 1; j++) {
            result[i][j] = (*this)[i < row ? i : i + 1][j < col ? j : j + 1];
        }
    }
    return result;
}

template<int size>
float Matrix<size>::Minor(int row, int col) const
{
    return Submatrix(row, col).Determinant();
}

template<int size>
float Matrix<size>::Cofactor(int row, int col) const
{
    return (((row + col) % 2 == 1) ? -1.0f : 1.0f) * Minor(row, col);
}

template<int size>
Matrix<size>& Matrix<size>::Invert(void)
{
    *this = Matrix<size>::Invert(*this);
    return *this;
}

template<int size>
Matrix<size> Matrix<size>::operator*(const Matrix<size>& rhs) const
{
    Matrix<size> result;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            result[i][j] = 0.0f;
            for(int k = 0; k < size; k++) {
                result[i][j] += (*this)[i][k] * rhs[k][j];
            }
        }
    }
    return result;
}

template<int size>
MatrixRow<size> Matrix<size>::operator[](int index)
{
    return MatrixRow<size>(m_Data + index * size);
}

template<int size>
const MatrixRow<size> Matrix<size>::operator[](int index) const
{
    return MatrixRow<size>(m_Data + index * size);
}

template<int size>
bool Matrix<size>::operator==(const Matrix<size>& rhs) const
{
    for(int i = 0; i < size * size; i++) {
        if(!Equal(m_Data[i], rhs.m_Data[i])) return false;
    }
    
    return true;
}

template<int size>
bool Matrix<size>::operator!=(const Matrix<size>& rhs) const
{
    return !(*this == rhs);
}

template<int size>
Matrix<size>::operator Matrix4(void)
{
    assert(size == 4);
    return Matrix4(m_Data);
}

template<int size>
std::ostream& operator<<(std::ostream& out, const Matrix<size>& rhs)
{
    for(int i = 0; i < size; i++) {
        out << "|";
        for(int j = 0; j < size; j++) {
            out.width(10); out << rhs[i][j];
            if(j < size - 1) out << " ";
        }
        out << "|\n";
    }
    out << std::defaultfloat;
    return out;
}

#pragma endregion

#pragma endregion