#ifndef VISCOSITYKERNEL_H
#define VISCOSITYKERNEL_H

#include "SPHKernel.h"

class ViscosityKernel : public SPHKernel
{
public:
    ViscosityKernel(const float & maxDist);

    virtual float operator()(const QVector<float> & R_ij)const;

    virtual QVector<float> gradient(const QVector<float> & R_ij)const;

    virtual float laplacian(const QVector<float> & R_ij)const;
};

#endif // VISCOSITYKERNEL_H
