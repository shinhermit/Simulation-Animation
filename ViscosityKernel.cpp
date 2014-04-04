#include "ViscosityKernel.h"

ViscosityKernel::ViscosityKernel(const float & maxDist)
    :SPHKernel(maxDist)
{}

float ViscosityKernel::operator ()(const QVector<float> & pos)const
{
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

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

QVector<float> ViscosityKernel::gradient(const QVector<float> & pos)const
{
    QVector<float> res;
    float coef;
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    if(dist <= _maxDist)
    {
        coef = -7.5*(-1.5*dist/_sqrMaxDist + 2/_maxDist - 0.5*_maxDist/std::pow(dist,3)) / std::pow(_maxDist, 3);

        res << coef*pos[0] << coef*pos[1] << coef*pos[2];
    }

    else
        res << 0 << 0 << 0;

    return res;
}

float ViscosityKernel::laplacian(const QVector<float> & pos)const
{
    float dist = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    if(dist <= _maxDist)
    {
        return ( 45*(_maxDist - dist) / std::pow(_maxDist, 6) );
    }

    else
        return 0;

}
