#ifndef POLY6_H
#define POLY6_H

#include <cmath>

#include "SPHKernel.h"

class Poly6Kernel : public SPHKernel
{
public:
    Poly6Kernel(const float & maxDist);

    virtual float operator()(const QVector<float> & pos)const;

    virtual QVector<float> gradient(const QVector<float> & pos)const;

    virtual float laplacian(const QVector<float> & pos)const;
};

#endif // POLY6_H
