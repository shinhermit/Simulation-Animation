#include "Poly6Kernel.h"

Poly6Kernel::Poly6Kernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

QVector<float> Poly6Kernel::operator ()(const QVector<float> & R_ij)const
{
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];

    if(sqrDist <= _sqrMaxDist)
    {
        return 315*std::pow(_sqrMaxDist - sqrDist, 3) / (64*SPHKernel::PI * std::pow(_maxDist, 9));
    }

    else
        return 0;
}
