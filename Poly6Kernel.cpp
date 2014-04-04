#include "Poly6Kernel.h"

Poly6Kernel::Poly6Kernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float Poly6Kernel::operator ()(const QVector<float> & pos)const
{
    float sqrDist = pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];

    if(sqrDist <= _maxDist)
    {
        return (4*std::pow(_sqrMaxDist - sqrDist, 3) / std::pow(_maxDist, 8));
    }

    else
        return 0;
}

QVector<float> Poly6Kernel::gradient(const QVector<float> & pos)const
{
    QVector<float> res;
    float coef;
    float sqrDist = pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];

    if(sqrDist <= _maxDist)
    {
        coef = -24*std::pow(_sqrMaxDist - sqrDist, 2) / std::pow(_maxDist, 8);

        res << coef*pos[0] << coef*pos[2] << coef*pos[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float Poly6Kernel::laplacian(const QVector<float> & pos)const
{
    float sqrDist = pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2];

    if(sqrDist <= _maxDist)
    {
        return ( -24*(_sqrMaxDist - sqrDist)*(3*_sqrMaxDist - 7*sqrDist) / std::pow(_maxDist, 8) );
    }

    else
        return 0;
}
