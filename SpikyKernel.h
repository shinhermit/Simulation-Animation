#ifndef SPIKYKERNEL_H
#define SPIKYKERNEL_H

#include "SPHKernel.h"

class SpikyKernel : public SPHKernel
{
public:
    SpikyKernel(const float & maxDist);

    virtual float operator()(const QVector<float> & pos)const;

    virtual QVector<float> gradient(const QVector<float> & pos)const;

    virtual float laplacian(const QVector<float> & pos)const;
};

#endif // SPIKYKERNEL_H
