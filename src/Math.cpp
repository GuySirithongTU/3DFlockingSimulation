#include "Math.h"

#include <cmath>
#include <cassert>

#pragma region tuple

Tuple::Tuple(void)
    : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Tuple::Tuple(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w) {}

float Tuple::Dot(const Tuple& lhs, const Tuple& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Tuple Tuple::operator+(const Tuple& rhs) const
{
    return Tuple(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

Tuple Tuple::operator-(const Tuple& rhs) const
{
    return Tuple(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

Tuple Tuple::operator-(void) const
{
    return Tuple(-x, -y, -z, -w);
}

Tuple Tuple::operator*(float rhs) const
{
    return Tuple(rhs * x, rhs * y, rhs * z, rhs * w);
}

bool Tuple::operator==(const Tuple& rhs) const
{
    return Equal(x, rhs.x) && Equal(y, rhs.y) && Equal(z, rhs.z) && Equal(w, rhs.w);
}

bool Tuple::operator!=(const Tuple& rhs) const
{
    return !(*this == rhs);
}

float& Tuple::operator[](int index)
{
    return *(&x + index);
}

float Tuple::operator[](int index) const
{
    return *(&x + index);
}

Tuple::operator Point(void)
{
    return Point(x, y, z);
}

Tuple::operator Vector(void)
{
    return Vector(x, y, z);
}

Tuple::operator Color(void)
{
    return Color(r, g, b);
}

Tuple operator*(float lhs, const Tuple& rhs)
{
    return rhs * lhs;
}

std::ostream& operator<<(std::ostream& out, const Tuple& tuple)
{
    return out << "[" << tuple.x << ", " << tuple.y << ", " << tuple.z << ", " << tuple.w << "]";
}

#pragma endregion

#pragma region point

Point::Point(void)
    : Tuple(0.0f, 0.0f, 0.0f, 1.0f) {}

Point::Point(float x, float y, float z)
    : Tuple(x, y, z, 1.0f) {}

#pragma endregion

#pragma region vector

Vector::Vector(void)
    : Tuple(0.0f, 0.0f, 0.0f, 0.0f) {}

Vector::Vector(float x, float y, float z)
    : Tuple(x, y, z, 0.0f) {}

Vector Vector::Normalize(const Vector& vec)
{
    float magnitude = vec.Magnitude();
    return Tuple(vec.x / magnitude, vec.y / magnitude, vec.z / magnitude, vec.w / magnitude);
}

Vector Vector::Cross(const Vector& lhs, const Vector& rhs)
{
    return Vector(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    );
}

Vector Vector::Reflect(const Vector& in, const Vector& normal)
{
    return in - 2.0f * normal * Tuple::Dot(in, normal);
}

float Vector::Magnitude(void) const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

Vector& Vector::Normalize(void)
{
    *this = Vector::Normalize(*this);
    return *this;
}

#pragma endregion

#pragma region color

Color::Color(void)
    : Tuple() {}

Color::Color(float r, float g, float b, float a)
    : Tuple(r, g, b, a) {}

Color Color::operator*(const Color& rhs) const
{
    return Color(x * rhs.x, y * rhs.y, z * rhs.z, a * rhs.a);
}

#pragma endregion

#pragma region matrix

template<>
float Matrix<2>::Determinant(void) const
{
    return (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];
}

Tuple operator*(const Matrix<4>& lhs, const Tuple& rhs)
{
    Tuple result;
    for(int i = 0; i < 4; i++) {
        result[i] = 0.0f;
        for(int j = 0; j < 4; j++) {
            result[i] += lhs[i][j] * rhs[j];
        }
    }
    return result;
}

Matrix4::Matrix4(void)
    : Matrix() {}

Matrix4::Matrix4(const float *data)
    : Matrix(data) {}

Matrix4::Matrix4(const Tuple& r0, const Tuple& r1, const Tuple& r2, const Tuple& r3)
{
    std::copy((const float *)&r0, (const float *)(&r0 + 1), m_Data); 
    std::copy((const float *)&r1, (const float *)(&r1 + 1), m_Data + 4);
    std::copy((const float *)&r2, (const float *)(&r2 + 1), m_Data + 8);
    std::copy((const float *)&r3, (const float *)(&r3 + 1), m_Data + 12);
}

Matrix4 Matrix4::Translate(float x, float y, float z)
{
    Matrix4 result = Matrix4::Identity();
    result[0][3] = x;
    result[1][3] = y;
    result[2][3] = z;
    return result;
}

Matrix4 Matrix4::Scale(float x, float y, float z)
{
    Matrix4 result = Matrix::Identity();
    result[0][0] = x;
    result[1][1] = y;
    result[2][2] = z;
    return result;
}

Matrix4 Matrix4::RotateX(float a)
{
    a = DEG_TO_RAD(a);
    Matrix4 result = Matrix::Identity();
    float sinA = sinf(a);
    float cosA = cosf(a);
    result[1][1] = cosA;
    result[1][2] = -sinA;
    result[2][1] = sinA;
    result[2][2] = cosA;
    return result;
}

Matrix4 Matrix4::RotateY(float a)
{
    a = DEG_TO_RAD(a);
    Matrix4 result = Matrix::Identity();
    float sinA = sinf(a);
    float cosA = cosf(a);
    result[0][0] = cosA;
    result[0][2] = sinA;
    result[2][0] = -sinA;
    result[2][2] = cosA;
    return result;
}

Matrix4 Matrix4::RotateZ(float a)
{
    a = DEG_TO_RAD(a);
    Matrix4 result = Matrix::Identity();
    float sinA = sinf(a);
    float cosA = cosf(a);
    result[0][0] = cosA;
    result[0][1] = -sinA;
    result[1][0] = sinA;
    result[1][1] = cosA;
    return result;
}

Matrix4 Matrix4::Shear(float xy, float xz, float yx, float yz, float zx, float zy)
{
    Matrix4 result = Matrix4::Identity();
    result[0][1] = xy;
    result[0][2] = xz;
    result[1][0] = yx;
    result[1][2] = yz;
    result[2][0] = zx;
    result[2][1] = zy;
    return result;
}

Matrix4 Matrix4::LookAt(const Point& eye, const Point& at, const Vector& up)
{
    Vector z = Vector::Normalize(eye - at);
    Vector y = Vector::Normalize(up);
    Vector x = Vector::Normalize(Vector::Cross(y, z));
    y = Vector::Normalize(Vector::Cross(z, x));

    float result[] = {
        x.x, x.y, x.z, -Tuple::Dot(x, eye),
        y.x, y.y, y.z, -Tuple::Dot(y, eye),
        z.x, z.y, z.z, -Tuple::Dot(z, eye),
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return Matrix4(result);
}

Matrix4 Matrix4::Perspective(float fovY, float aspect, float zNear, float zFar)
{
    float near = 1.0f / tanf(DEG_TO_RAD(fovY) / 2.0f);
    float result[] = {
        near / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, near, 0.0f, 0.0f,
        0.0f, 0.0f, -(zFar + zNear) / (zFar - zNear), (-2.0f * zNear * zFar) / (zFar - zNear),
        0.0f, 0.0f, -1.0f, 0.0f
    };
    return Matrix4(result);
}

Matrix4 Matrix4::Frustum(float left, float right, float bottom, float top, float near, float far)
{
    float result[] = {
        2.0f * near / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
        0.0f, 2.0f * near / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -2.0f * far * near / (far - near),
        0.0f, 0.0f, -1.0f, 0.0f
    };
    return Matrix4(result);
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    float result[] = {
        2.0f * (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
        0.0f, 2.0f * (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
        0.0f, 0.0f, -2.0f * (far - near), -(far + near) / (far - near),
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return Matrix4(result);
}

#pragma endregion