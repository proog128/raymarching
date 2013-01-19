#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <algorithm>
#include <stdint.h>

template<typename TYPE, int N>
class Vector
{
    typedef Vector<TYPE, N> VTYPE;
public:
    Vector()
    {
        std::fill(std::begin(v), std::end(v), (TYPE)0);
    }

    explicit Vector(const TYPE& v0)
    {
        std::fill(std::begin(v), std::end(v), v0);
    }

    Vector(const TYPE& v0, const TYPE& v1)
    {
        v[0] = v0;
        v[1] = v1;
    }

    Vector(const TYPE& v0, const TYPE& v1, const TYPE& v2)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }

    Vector(const TYPE& v0, const TYPE& v1, const TYPE& v2, const TYPE& v3)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        v[3] = v3;
    }
    
    const TYPE& operator[](int i) const
    {
        return v[i];
    }

    TYPE& operator[](int i)
    {
        return v[i];
    }

    const TYPE& operator()(int i) const
    {
        return v[i];
    }

    TYPE& operator()(int i)
    {
        return v[i];
    }

    VTYPE operator*(TYPE a) const
    {
        VTYPE r;
        for(int i=0; i<N; ++i)
            r[i] = v[i] * a;
        return r;
    }

    VTYPE operator+(TYPE a) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] + a;
        return result;
    }

    VTYPE operator-(TYPE a) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] - a;
        return result;
    }

    VTYPE operator*(const VTYPE& x) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] * x[i];
        return result;
    }

    VTYPE operator/(const VTYPE& x) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] / x[i];
        return result;
    }

    VTYPE operator+(const VTYPE& x) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] + x[i];
        return result;
    }

    VTYPE operator-(const VTYPE& x) const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = v[i] - x[i];
        return result;
    }

    VTYPE& operator+=(const VTYPE& x)
    {
        *this = *this + x;
        return *this;
    }

    VTYPE& operator-=(const VTYPE& x)
    {
        *this = *this - x;
        return *this;
    }

    VTYPE& operator*=(const VTYPE& x)
    {
        *this = *this * x;
        return *this;
    }

    VTYPE& operator/=(const VTYPE& x)
    {
        *this = *this / x;
        return *this;
    }

    VTYPE cross(const VTYPE& x);
    
    VTYPE operator-() const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = -v[i];
        return result;
    }

    TYPE dot(const VTYPE& x) const
    {
        TYPE result = (TYPE)0;
        for(int i=0; i<N; ++i)
            result += v[i] * x[i];
        return result;
    }

    VTYPE abs() const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = ::abs(v[i]);
        return result;
    }

    VTYPE floor() const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = ::floor(v[i]);
        return result;
    }

    VTYPE ceil() const
    {
        VTYPE result;
        for(int i=0; i<N; ++i)
            result[i] = ::ceil(v[i]);
        return result;
    }

    float sqrlength() const
    {
        TYPE sum = 0.f;
        for(int i=0; i<N; ++i)
            sum += v[i]*v[i];
        return sum;
    }

    float length() const
    {
        return sqrt(sqrlength());
    }

    void normalize()
    {
        float linv = 1./length();
        for(int i=0; i<N; ++i)
            v[i] = v[i] * linv;
    }


private:
    TYPE v[N];
};

typedef Vector<int8_t, 2> Vec2b;
typedef Vector<int8_t, 3> Vec3b;
typedef Vector<int8_t, 4> Vec4b;

typedef Vector<uint8_t, 2> Vec2ub;
typedef Vector<uint8_t, 3> Vec3ub;
typedef Vector<uint8_t, 4> Vec4ub;

typedef Vector<int, 2> Vec2i;
typedef Vector<int, 3> Vec3i;
typedef Vector<int, 4> Vec4i;

typedef Vector<float, 2> Vec2f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

typedef Vector<double, 2> Vec2d;
typedef Vector<double, 3> Vec3d;
typedef Vector<double, 4> Vec4d;

template<>
inline Vector<float, 3> Vector<float, 3>::cross(const Vector<float, 3>& x)
{
    return Vector<float, 3>(v[1]*x[2] - v[2]*x[1],
                            v[2]*x[0] - v[0]*x[2],
                            v[0]*x[1] - v[1]*x[0]);
}

template<typename TYPE, int N>
Vector<TYPE, N> operator*(TYPE a, const Vector<TYPE, N>& vec)
{
    return vec * a;
}

template<typename TYPE, int N>
Vector<TYPE, N> operator+(TYPE a, const Vector<TYPE, N>& vec)
{
    return vec + a;
}


template<typename TYPE, int N>
Vector<TYPE, N> operator-(TYPE a, const Vector<TYPE, N>& vec)
{
    return -vec;// + a;
}

template<typename TYPE, int N>
std::ostream& operator<<(std::ostream& out, const Vector<TYPE, N>& vec)
{
    out << "[";
    for(int i=0; i<N-1; ++i)
    {
        out << vec[i] << " ";
    }
    out << vec[N-1] << "]";
    return out;
}

#endif
