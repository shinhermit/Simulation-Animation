#ifndef POLY6_H
#define POLY6_H

#include <cmath>

#include "SPHKernel.h"

class Poly6Kernel : public SPHKernel
{
public:
    Poly6Kernel(const float & maxDist);

    virtual QVector<float>  operator()(const QVector<float> & R_ij)const;
};

#endif // POLY6_H
