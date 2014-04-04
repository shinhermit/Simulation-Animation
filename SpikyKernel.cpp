#include "SpikyKernel.h"

SpikyKernel::SpikyKernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float SpikyKernel::operator ()(const QVector<float> & pos) const
{
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    if(dist <= _maxDist)
    {
        return ( 15*std::pow(_maxDist - dist, 3) / std::pow(_maxDist, 6) );
    }

    else
        return 0;
}

QVector<float> SpikyKernel::gradient(const QVector<float> & pos)const
{
    QVector<float> res;
    float coef;
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    if(dist <= _maxDist)
    {
        coef = -45*std::pow(_maxDist/dist - 1, 2) / std::pow(_maxDist, 6);

        res << coef*pos[0] << coef*pos[1] << coef*pos[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float SpikyKernel::laplacian(const QVector<float> & pos)const
{
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    if(dist <= _maxDist)
    {
        return ( -45*(1 - 2*dist + 3*dist/_sqrMaxDist ) / (dist*std::pow(_maxDist, 4)) );
    }

    else
        return 0;
}
