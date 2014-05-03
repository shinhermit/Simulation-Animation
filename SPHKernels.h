#ifndef SPHKERNEL_H
#define SPHKERNEL_H

#include <stdexcept>
#include <cmath>

#include <QVector>
#include <QObject>

/// This class provides static methods that implement 3 SPH smoothing kernels
class SPHKernels
{
public:
    static const float PI; /*!< Constant Pi */

    /// \brief Poly6 kernel, for density computation
    static float poly6(const float & maxDist, const QVector<float> & R_ij);
    /// \brief Spiky kernel, for pressure influence computation
    static QVector<float> spiky(const float & maxDist, const QVector<float> & R_ij);
    /// \brief Kernel for viscosity influence computation
    static float visco(const float & maxDist, const QVector<float> & R_ij);
};

#endif // SPHKERNEL_H
