#ifndef SPHKERNELS_H
#define SPHKERNELS_H

#include <stdexcept>
#include <cmath>

#include <QVector>
#include <QObject>

class SPHKernels
{
public:
    static const float PI;

    static float poly6(const float & maxDist, const QVector<float> & R_ij);
    static QVector<float> spiky(const float & maxDist, const QVector<float> & R_ij);
    static float visco(const float & maxDist, const QVector<float> & R_ij);

private:
    static float _maxDist;
    static float _sqrMaxDist;
};

#endif // SPHKERNELS_H
