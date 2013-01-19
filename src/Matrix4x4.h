#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <vector>
#include <algorithm>
#include "Vector.h"

template<typename TYPE>
class Matrix4x4
{
public:
    Matrix4x4()
    {
        std::fill(std::begin(v), std::end(v), (TYPE)0);
    }

    Matrix4x4(const TYPE& v)
    {
        std::fill(std::begin(v), std::end(v), v0);
    }

    Matrix4x4(const TYPE& v00, const TYPE& v01, const TYPE& v02, const TYPE& v03,
              const TYPE& v10, const TYPE& v11, const TYPE& v12, const TYPE& v13,
              const TYPE& v20, const TYPE& v21, const TYPE& v22, const TYPE& v23,
              const TYPE& v30, const TYPE& v31, const TYPE& v32, const TYPE& v33)
    {
        _(0,0) = v00; _(0,1) = v01; _(0,2) = v02; _(0,3) = v03;
        _(1,0) = v10; _(1,1) = v11; _(1,2) = v12; _(1,3) = v13;
        _(2,0) = v20; _(2,1) = v21; _(2,2) = v22; _(2,3) = v23;
        _(3,0) = v30; _(3,1) = v31; _(3,2) = v32; _(3,3) = v33;
    }

    const TYPE& operator()(int i, int j) const
    {
        return _(i,j);
    }

    TYPE& operator()(int i, int j)
    {
        return _(i,j);
    }

    TYPE& operator[](int i)
    {
        return v[i];
    }

    Matrix4x4<TYPE>& operator=(const Matrix4x4<TYPE>& x)
    {
        std::copy(x.v, v);
        return *this;
    }

    static Matrix4x4<TYPE> Identity()
    {
        Matrix4x4 mat;
        mat.v[0]  = 1; mat.v[1]  = 0; mat.v[2]  = 0; mat.v[3]  = 0;
        mat.v[4]  = 0; mat.v[5]  = 1; mat.v[6]  = 0; mat.v[7]  = 0;
        mat.v[8]  = 0; mat.v[9]  = 0; mat.v[10] = 1; mat.v[11] = 0;
        mat.v[12] = 0; mat.v[13] = 0; mat.v[14] = 0; mat.v[15] = 1;
        return mat;
    }

    Matrix4x4<TYPE>& RotateX(float alpha)
    {
	    TYPE ca = cos(alpha);
	    TYPE sa = sin(alpha);
        TYPE tmp;

	    tmp = _(0,1);
	    _(0,1) = tmp * ca - _(0,2) * sa;
	    _(0,2) = _(0,2) * ca + tmp * sa;

	    tmp = _(1,1);
	    _(1,1) = tmp * ca - _(1,2) * sa;
	    _(1,2) = _(1,2) * ca + tmp * sa;

	    tmp = _(2,1);
	    _(2,1) = tmp * ca - _(2,2) * sa;
	    _(2,2) = _(2,2) * ca + tmp * sa;

	    tmp = _(3,1);
	    _(3,1) = tmp * ca - _(3,2) * sa;
	    _(3,2) = _(3,2) * ca + tmp * sa;

        return *this;
    }

    Matrix4x4<TYPE>& RotateY(float alpha)
    {
	    TYPE ca = cos(alpha);
	    TYPE sa = sin(alpha);
        TYPE tmp;

	    tmp = _(0,0);
	    _(0,0) = tmp * ca + _(0,2) * sa;
	    _(0,2) = -tmp * sa + _(0,2) * ca;

	    tmp = _(1,0);
	    _(1,0) = tmp * ca + _(1,2) * sa;
	    _(1,2) = -tmp * sa + _(1,2) * ca;

	    tmp = _(2,0);
	    _(2,0) = tmp * ca + _(2,2) * sa;
	    _(2,2) = -tmp * sa + _(2,2) * ca;

	    tmp = _(3,0);
	    _(3,0) = tmp * ca + _(3,2) * sa;
	    _(3,2) = -tmp * sa + _(3,2) * ca;

        return *this;
    }

    Matrix4x4<TYPE>& RotateZ(float alpha)
    {
	    TYPE ca = cos(alpha);
	    TYPE sa = sin(alpha);
        TYPE tmp;

        tmp = _(0,0);
        _(0,0) = tmp * ca - _(0,1) * sa;
        _(0,1) = _(0,1) * ca + tmp * sa;

        tmp = _(1,0);
        _(1,0) = tmp * ca - _(1,1) * sa;
        _(1,1) = _(1,1) * ca + tmp * sa;

        tmp = _(2,0);
        _(2,0) = tmp * ca - _(2,1) * sa;
        _(2,1) = _(2,1) * ca + tmp * sa;

        tmp = _(3,0);
        _(3,0) = tmp * ca - _(3,1) * sa;
        _(3,1) = _(3,1) * ca + tmp * sa;

        return *this;
    }

    Matrix4x4<TYPE>& Translate(const Vector<TYPE, 3>& vec)
    {
        return Translate(vec[0], vec[1], vec[2]);
    }

    Matrix4x4<TYPE>& Translate(TYPE x, TYPE y, TYPE z)
    {
	    _(0,3) = _(0,0) * x + _(0,1) * y + _(0,2) * z + _(0,3);
	    _(1,3) = _(1,0) * x + _(1,1) * y + _(1,2) * z + _(1,3);
	    _(2,3) = _(2,0) * x + _(2,1) * y + _(2,2) * z + _(2,3);
	    _(3,3) = _(3,0) * x + _(3,1) * y + _(3,2) * z + _(3,3);
        return *this;
    }

    Matrix4x4<TYPE>& Rotate(float alpha, const Vector<TYPE, 3>& vec)
    {
        return Rotate(alpha, vec[0], vec[1], vec[2]);
    }

    Matrix4x4<TYPE>& Rotate(float alpha, TYPE x, TYPE y, TYPE z)
    {
	    Matrix4x4<TYPE> m;
	    TYPE mag, s, c;
	    TYPE xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	    mag = sqrt(x * x + y * y + z * z);
	    if(mag == 0.)
	    {
		    return *this;
	    }

	    s = sin(alpha);
	    c = cos(alpha);

	    x /= mag;
	    y /= mag;
	    z /= mag;

	    xx = x * x;
	    yy = y * y;
	    zz = z * z;
	    xy = x * y;
	    yz = y * z;
	    zx = z * x;
	    xs = x * s;
	    ys = y * s;
	    zs = z * s;
	    one_c = 1.0F - c;

	    m(0,0) = (one_c * xx) + c;
	    m(0,1) = (one_c * xy) - zs;
	    m(0,2) = (one_c * zx) + ys;
	    m(0,3) = 0.0F;

	    m(1,0) = (one_c * xy) + zs;
	    m(1,1) = (one_c * yy) + c;
	    m(1,2) = (one_c * yz) - xs;
	    m(1,3) = 0.0F;

	    m(2,0) = (one_c * zx) - ys;
	    m(2,1) = (one_c * yz) + xs;
	    m(2,2) = (one_c * zz) + c;
	    m(2,3) = 0.0F;

	    m(3,0) = 0.0F;
	    m(3,1) = 0.0F;
	    m(3,2) = 0.0F;
	    m(3,3) = 1.0F;

	    return operator*=(m);
    }

    Vector<TYPE, 3> operator*(const Vector<TYPE, 3>& vec)
    {
        TYPE x = _(0,0) * vec[0] + _(0,1) * vec[1] + _(0,2) * vec[2] + _(0,3);
        TYPE y = _(1,0) * vec[0] + _(1,1) * vec[1] + _(1,2) * vec[2] + _(1,3);
        TYPE z = _(2,0) * vec[0] + _(2,1) * vec[1] + _(2,2) * vec[2] + _(2,3);
        TYPE w = _(3,0) * vec[0] + _(3,1) * vec[1] + _(3,2) * vec[2] + _(3,3);

        return Vector<TYPE, 3>(x/w, y/w, z/w);
    }

    Matrix4x4<TYPE>& operator*=(const Matrix4x4<TYPE>& mat)
    {
        for(int i=0; i<4; ++i)
        {
            //TYPE _i0 = _(i,0), _i1 = _(i,1), _i2 = _(i,2), _i3 = _(i,3);
            //_(i,0) = 
            TYPE _i[4] = {_(i,0), _(i,1), _(i,2), _(i,3)};
            for(int j=0; j<4; ++j)
            {
                _(i,j) = 0;
                for(int k=0; k<4; ++k)
                {
                    _(i,j) += _i[k] * mat(k,j);
                }
            }
        }
        return *this;
    }

private:
    const TYPE& _(int i, int j) const
    {
        return v[i*4+j];
    }

    TYPE& _(int i, int j)
    {
        return v[i*4+j];
    }

    TYPE v[16];  // row-major
};

typedef Matrix4x4<int> Matrix4x4i;
typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix4x4d;

template<typename TYPE>
std::ostream& operator<<(std::ostream& out, const Matrix4x4<TYPE>& mat)
{
    out << "[";
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            out << mat(i,j) << " ";
        }
        out << mat(i,3) << ";\n ";
    }
    for(int j=0; j<3; ++j)
        out << mat(3,j) << " ";
    out << mat(3,3) << "]\n";
    return out;
}

#endif
