#ifndef VISCOSITYKERNEL_H
#define VISCOSITYKERNEL_H

#include "SPHKernel.h"

class ViscosityKernel : public SPHKernel
{
public:
    ViscosityKernel(const float & maxDist);

    virtual float operator()(const QVector<float> & pos)const;

    virtual QVector<float> gradient(const QVector<float> & pos)const;

    virtual float laplacian(const QVector<float> & pos)const;
};

#endif // VISCOSITYKERNEL_H
