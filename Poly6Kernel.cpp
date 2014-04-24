#include "Poly6Kernel.h"

Poly6Kernel::Poly6Kernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float Poly6Kernel::operator ()(const QVector<float> & R_ij)const
{
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];

    if(sqrDist <= _sqrMaxDist)
    {
        return std::pow(_sqrMaxDist - sqrDist, 3) / (3.1415 * std::pow(_maxDist, 8));
    }

    else
        return 0;
}

QVector<float> Poly6Kernel::gradient(const QVector<float> & R_ij)const
{
    QVector<float> res;
    float coef;
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];

    if(sqrDist <= _sqrMaxDist)
    {
        coef = -24*std::pow(_sqrMaxDist - sqrDist, 2) / (3.1415 * std::pow(_maxDist, 8));

        res << coef*R_ij[0] << coef*R_ij[1] << coef*R_ij[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float Poly6Kernel::laplacian(const QVector<float> & R_ij)const
{
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];

    if(sqrDist <= _sqrMaxDist)
    {
        return -48*(_sqrMaxDist - sqrDist)*(_sqrMaxDist - 3*sqrDist) / (3.1415*std::pow(_maxDist, 8));
    }

    else
        return 0;
}
