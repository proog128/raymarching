#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include "Vector.h"

struct DistanceField
{
    DistanceField(int w, int h, int d)
        : width(w), height(h), depth(d), data(w*h*d)
    {}

    int width;
    int height;
    int depth;

    float at(int x, int y, int z) const
    {
        return data[z*width*height + y*width + x];
    }

    float& at(int x, int y, int z)
    {
        return data[z*width*height + y*width + x];
    }

    std::vector<float> data;
};

DistanceField buildWorld();

#endif
