#include "SPHKernels.h"

const float SPHKernels::PI = 3.14159265359;

float SPHKernels::poly6(const float & h, const QVector<float> & R_ij)
{
    float r2 = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];
    float r = ::sqrt(r2);
    float h2 = h*h;

    if(r <= h)
        return 315.*(::pow(h2-r2, 3)) / (64.*SPHKernels::PI*::pow(h, 9));
    else
        return 0.;

}

QVector<float> SPHKernels::spiky(const float & h, const QVector<float> & R_ij)
{
    QVector<float> res;
    float r = ::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);
    float coef;

    if(r <= h)
        coef = 45.*::pow(h-r,3) / (r*SPHKernels::PI*::pow(h,6));
    else
        coef = 0.;

    res << coef*R_ij[0] << coef*R_ij[1] << coef*R_ij[2];

    return res;
}

float SPHKernels::visco(const float & h, const QVector<float> & R_ij)
{
    float r = ::sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);

    if(r <= h)
        return 45.*(h-r) / (SPHKernels::PI*::pow(h,6));
    else
        return 0.;
}
