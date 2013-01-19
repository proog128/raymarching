#ifndef NOISE_H_
#define NOISE_H_

#include "Vector.h"

template<int GRID_SIZE>
class PerlinNoise
{
public:
    PerlinNoise(unsigned int seed)
         : p(NULL), rndState(seed)
    {
        initPermutation();
    }

    ~PerlinNoise()
    {
    }

    float operator()(float x, float y, float z)
    {
        // find unit cube that contains the point (256x256x256 grid)
        int X = (int)x % GRID_SIZE,
            Y = (int)y % GRID_SIZE,
            Z = (int)z % GRID_SIZE;

        // find relative coordinates of point in unit cube
        x -= (int)x;
        y -= (int)y;
        z -= (int)z;

        // find hash values for each of the 8 cube coordinates
        int A = p[X] + Y,
            AA = p[A] + Z,
            AB = p[A+1] + Z,
            B = p[X+1] + Y,
            BA = p[B] + Z,
            BB = p[B+1] + Z;

        // compute hermite curve for x,y,z
        float u = smootherstep(x),
              v = smootherstep(y),
              w = smootherstep(z);

        float a = mix(u, gradientDotVec(p[AA], x, y, z), gradientDotVec(p[BA], x-1, y, z));
        float b = mix(u, gradientDotVec(p[AB], x, y-1, z), gradientDotVec(p[BB], x-1, y-1, z));
        float ab = mix(v, a, b);

        float c = mix(u, gradientDotVec(p[AA+1], x, y, z-1), gradientDotVec(p[BA+1], x-1, y, z-1));
        float d = mix(u, gradientDotVec(p[AB+1], x, y-1, z-1), gradientDotVec(p[BB+1], x-1, y-1, z-1));
        float cd = mix(v, c, d);

        float abcd = mix(w, ab, cd);

        return abcd;
    }

private:
    void initPermutation()
    {
        p = new unsigned int[2*GRID_SIZE];
        for(int i=0; i<2*GRID_SIZE; ++i)
            p[i] = rand();
    }

    float gradientDotVec(int hash, float x, float y, float z)
    {
        int h = hash & 0xF;
        
         // Compute dot product between the gradient vector corresponding 
         // to h and (x,y,z)
         float u = h < 8 ? x : y;
         float v = h < 4 ? y : (h==12 || h==14 ? x : z);
         return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
    }

    unsigned int rand()        
    {
        rndState = (1664525 * rndState + 1013904223);
        return (rndState >> 16) % GRID_SIZE;
    }

    float smootherstep(float t)
    {
        return t*t*t*(t*(t*6.f-15.f)+10.f);
    }

    float mix(float t, float a, float b)
    {
        return a+t*(b-a);
    }

    unsigned int rndState;
    unsigned int* p;
};

template<class NOISE_FUNC>
float turbulence(float x, float y, float z, float a, float b, int l0, int lmax, NOISE_FUNC n)
{
    float sum = 0.f;
    for(int i=l0; i<lmax; ++i)
    {
        float f = pow(a, i);//(1 << i);
        float pui = pow(b, i);   // 2^i
        sum += n(f*x, f*y, f*z) / pui;
    }
    return sum;
}

#endif
