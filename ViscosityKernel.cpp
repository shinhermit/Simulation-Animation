#include "ViscosityKernel.h"

ViscosityKernel::ViscosityKernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float ViscosityKernel::operator ()(const QVector<float> & R_ij)const
{
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);

    if(dist <= _maxDist)
    {
        float cube = -0.5*std::pow(dist,3) / std::pow(_maxDist,3);
        float square = dist*dist / _sqrMaxDist;
        float inv = 0.5*_maxDist / dist;

        return ( 7.5*(cube+square+inv-1) / std::pow(_maxDist, 3) );
    }

    else
        return 0;
}

QVector<float> ViscosityKernel::gradient(const QVector<float> & R_ij)const
{
    QVector<float> res;
    float coef;
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);

    if(dist <= _maxDist)
    {
        coef = -7.5*(-1.5*dist/_sqrMaxDist + 2/_maxDist - 0.5*_maxDist/std::pow(dist,3)) / std::pow(_maxDist, 3);

        res << coef*R_ij[0] << coef*R_ij[1] << coef*R_ij[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float ViscosityKernel::laplacian(const QVector<float> & R_ij)const
{
    float dist = std::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);
    float q = dist/_maxDist;

    if(dist <= _maxDist)
    {
        return 40*(1 - q) / (3.1415*std::pow(_maxDist, 4));
    }

    else
        return 0;
}
