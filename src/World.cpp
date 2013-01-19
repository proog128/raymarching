#include "World.h"
#include <cassert>
#include "noise.h"
#include "Timer.h"
#include <iostream>

int DEFAULT_WORLD_WIDTH = 256;
int DEFAULT_WORLD_HEIGHT = 256;
int DEFAULT_WORLD_DEPTH = 256;

PerlinNoise<256> gPerlin(42);

struct VectorDistanceFieldFloat
{
    VectorDistanceFieldFloat(int w, int h, int d)
        : width(w), height(h), depth(d), data(w*h*d)
    {}

    int width;
    int height;
    int depth;

    Vec3f at(int x, int y, int z) const
    {
        return data[z*width*height + y*width + x];
    }

    Vec3f at(const Vec3i& pos) const
    {
        return at(pos[0], pos[1], pos[2]);
    }

    Vec3f& at(int x, int y, int z)
    {
        return data[z*width*height + y*width + x];
    }

    Vec3f& at(const Vec3i& pos)
    {
        return at(pos[0], pos[1], pos[2]);
    }

    std::vector<Vec3f> data;
};

template<class T>
T clamp(const T& x, const T& l, const T& u)
{
    return std::min(u, std::max(x, l));
}

bool isSolid(float x, float y, float z, int w, int h, int d)
{
    float xx = clamp<float>(x, 0, w-1);
    float yy = clamp<float>(y, 0, h-1);
    float zz = clamp<float>(z, 0, d-1);

    float density = y-80.f;
    density += gPerlin(128.f*x/w, 128.f*y/h, 128.f*z/d) * 2.5f;
    density += gPerlin(64.f*x/w, 64.f*y/h, 64.f*z/d) * 2.5f;
    density += gPerlin(32.f*x/w, 32.f*y/h, 32.f*z/d) * 5.f;
    density += gPerlin(6.f*x/w, 6.f*y/h, 6.f*z/d) * 70.f;
    density += gPerlin(4.f*x/w, 4.f*y/h, 4.f*z/d) * 100.f;
    density += gPerlin(2.f*x/w, 2.f*y/h, 2.f*z/d) * 100.f;
    density += gPerlin(1.f*x/w, 1.f*y/h, 1.f*z/d) * 100.f;

    bool solid = density < 0.f;
    
    return solid;
}

bool isSolid(const Vec3f& point, int w, int h, int d)
{
    return isSolid(point[0], point[1], point[2], w, h, d);
}

bool isSolid(const Vec3i& point, int w, int h, int d)
{
    return isSolid(point[0], point[1], point[2], w, h, d);
}

bool findBorder(const Vec3i& point, const Vec3i& delta, int w, int h, int d, float& pos)
{
    bool b0 = isSolid(point, w, h, d);
    bool b1 = isSolid(point+delta, w, h, d);
    if(b0 != b1)
    {
        Vec3f pointf(point[0], point[1], point[2]);
        Vec3f deltaf(delta[0], delta[1], delta[2]);

        float delta = 0.5f;
        pos = 0.5f;
        for(int i=0; i<10; ++i)
        {
            Vec3f ppf = pointf + pos * deltaf;
            delta *= 0.5f;
            if(b0 == isSolid(ppf, w, h, d))
            {
                pos += delta;
            }
            else
            {
                pos -= delta;
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

void minVec(Vec3f& dest, const Vec3f& other)
{
    float destLength = dest.sqrlength();
    float otherLength = other.sqrlength();
    if(otherLength < destLength)
    {
        dest = other;
        Vec3f absDest = dest.abs();
        if(absDest[1] > absDest[0]) 
            std::swap(dest[1], dest[0]);
        if(absDest[2] > absDest[1]) 
            std::swap(dest[2], dest[1]);
        if(absDest[1] > absDest[0]) 
            std::swap(dest[1], dest[0]);
    }
}

Vec3f minVecBounded(VectorDistanceFieldFloat& vdf, const Vec3i& bounds, const Vec3i& coords, const Vec3f& delta)
{
    int w = bounds[0];
    int h = bounds[1];
    int d = bounds[2];

    int x = coords[0];
    int y = coords[1];
    int z = coords[2];

    int xx = x + delta[0];
    int yy = y + delta[1];
    int zz = z + delta[2];

    Vec3f V0 = vdf.at(x,y,z);
    if(xx >= 0 && xx < w && yy >= 0 && yy < h && zz >= 0 && zz < d)
    {
        Vec3f V1 = vdf.at(xx, yy, zz) + delta.abs();

        float V0l = (float)V0[0]*V0[0] + V0[1]*V0[1] + V0[2]*V0[2];
        float V1l = (float)V1[0]*V1[0] + V1[1]*V1[1] + V1[2]*V1[2];

        Vec3f V = V0l < V1l ? V0 : V1;

        if(V[1] > V[0]) 
            std::swap(V[1], V[0]);
        if(V[2] > V[1]) 
            std::swap(V[2], V[1]);
        if(V[1] > V[0]) 
            std::swap(V[1], V[0]);

        return V;
    }
    else
    {
        return V0;
    }
}

void scan(VectorDistanceFieldFloat& vdf, const Vec3i& delta)
{
    int sx = delta[0] < 0 ? -delta[0] : 0;
    int ex = delta[0] > 0 ? vdf.width - delta[0] : vdf.width;
    int sy = delta[1] < 0 ? -delta[1] : 0;
    int ey = delta[1] > 0 ? vdf.height - delta[1] : vdf.height;
    int sz = delta[2] < 0 ? -delta[2] : 0;
    int ez = delta[2] > 0 ? vdf.depth - delta[2] : vdf.depth;

#pragma omp parallel for
    for(int z=sz; z<ez; ++z)
    {
        for(int y=sy; y<ey; ++y)
        {
            for(int x=sx; x<ex; ++x)
            {
                float borderPos;
                Vec3i pos(x,y,z);
                if(findBorder(pos, delta, vdf.width, vdf.height, vdf.depth, borderPos))
                {
                    Vec3f deltaf(delta[0], delta[1], delta[2]);
                    minVec(vdf.at(pos), deltaf * borderPos);
                    minVec(vdf.at(pos+delta), deltaf * /*(-1)**/(1-borderPos));
                }
            }
        }
    }
}

VectorDistanceFieldFloat initVDT(int width, int height, int depth)
{
    VectorDistanceFieldFloat vectorDistanceField(width,height,depth);
    std::fill(vectorDistanceField.data.begin(), vectorDistanceField.data.end(), Vec3f(999999.f,999999.f,999999.f));

    std::cout << "Init VDT... " << std::flush;
    Timer t;

    scan(vectorDistanceField, Vec3i(1, 0, 0));    std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(0, 1, 0));    std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(0, 0, 1));    std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(1, 1, 0));    std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(-1, 1, 0));   std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(0, 1, 1));    std::cout << "." << std::flush;
    scan(vectorDistanceField, Vec3i(0, -1, 1));   std::cout << "." << std::flush;

    t.stop();

    return vectorDistanceField;
}

DistanceField VDT(VectorDistanceFieldFloat& vectorDistanceField)
{
    int w = vectorDistanceField.width, h = vectorDistanceField.height, d = vectorDistanceField.depth;
    Vec3i bounds(w, h, d);

    std::cout << "VDT: forward..." << std::flush;
    Timer t;

    // forward pass
    for(int z=0; z<d; ++z)
    {
        auto slice = vectorDistanceField.data.begin() + z*w*h;

        for(int y=0; y<h; ++y)
        {
            auto row = slice + y*w;

            for(int x=0; x<w; ++x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(-1.f, 0, 0));
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, -1.f, 0));
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, 0, -1.f));
            }

            for(int x=w-1; x>=0; --x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(1.f, 0, 0));
            }
        }

        for(int y=h-1; y>=0; --y)
        {
            auto row = slice + y*w;

            for(int x=w-1; x>=0; --x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, 1.f, 0));
            }
        }
    }
    t.stop();

    std::cout << "VDT: backward..." << std::flush;
    t.start();

    // backward pass
    for(int z=d-1; z>=0; --z)
    {
        auto slice = vectorDistanceField.data.begin() + z*w*h;

        for(int y=h-1; y>=0; --y)
        {
            auto row = slice + y*w;

            for(int x=w-1; x>=0; --x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(1.f, 0, 0));
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, 1.f, 0));
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, 0, 1.f));
            }

            for(int x=0; x<w; ++x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(-1.f, 0, 0));
            }
        }

        for(int y=0; y<h; ++y)
        {
            auto row = slice + y*w;

            for(int x=0; x<w; ++x)
            {
                Vec3f& L = *(row + x);
                L = minVecBounded(vectorDistanceField, bounds, Vec3i(x, y, z), Vec3f(0, -1.f, 0));
            }
        }
    }
    t.stop();

    std::cout << "VDT-SDT conversion..." << std::flush;
    t.start();

    DistanceField distanceField(w, h, d);

    for(int z=0; z<d; ++z)
    {
        auto slice = distanceField.data.begin() + z*w*h;
        auto sliceVDF = vectorDistanceField.data.begin() + z*w*h;

        for(int y=0; y<h; ++y)
        {
            auto row = slice + y*w;
            auto rowVDF = sliceVDF + y*w;

            for(int x=0; x<w; ++x)
            {
                const Vec3f& v = *(rowVDF + x);
                bool solid = isSolid(x, y, z, w, h, d);
                Vec3f s((float)v[0] / w, (float)v[1] / h, (float)v[2] / d);
                *(row + x) = sqrtf((float)s[0]*s[0] + s[1]*s[1] + s[2]*s[2]) * (solid ? -1 : 1);
            }
        }
    }
    t.stop();

    return distanceField;
}

DistanceField buildWorld()
{
    auto vectorDistanceField = initVDT(DEFAULT_WORLD_WIDTH, DEFAULT_WORLD_HEIGHT, DEFAULT_WORLD_DEPTH);
    auto distanceField = VDT(vectorDistanceField);

    return distanceField;
}
