#include "SpikyKernel.h"

SpikyKernel::SpikyKernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float SpikyKernel::operator ()(const QVector<float> & R_ij) const
{
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);

    if(dist <= _maxDist)
    {
        return ( 15*std::pow(_maxDist - dist, 3) / std::pow(_maxDist, 6) );
    }

    else
        return 0;
}

QVector<float> SpikyKernel::gradient(const QVector<float> & R_ij)const
{
    QVector<float> res;
    float coef;
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);
    float q = dist/_maxDist;

    if(dist <= _maxDist)
    {
        coef = -30*std::pow(1-q, 2) / (SPHKernel::PI*q*std::pow(_maxDist, 4));

        res << coef*R_ij[0] << coef*R_ij[1] << coef*R_ij[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float SpikyKernel::laplacian(const QVector<float> & R_ij)const
{
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);

    if(dist <= _maxDist)
    {
        return ( -45*(1 - 2*dist + 3*dist/_sqrMaxDist ) / (dist*std::pow(_maxDist, 4)) );
    }

    else
        return 0;
}
