#ifndef SPHKERNEL_H
#define SPHKERNEL_H

#include <stdexcept>
#include <cmath>

#include <QVector>
#include <QObject>

class SPHKernel
{
public:
    SPHKernel(const float & maxDist=1.0);
    virtual ~SPHKernel();

    void setMaxInfluenceDistance(const float & maxDist) throw(std::invalid_argument);

    float getMaxInfluenceDistance()const;

    virtual float operator()(const QVector<float> & R_ij)const=0;

    virtual QVector<float> gradient(const QVector<float> & R_ij)const=0;

    virtual float laplacian(const QVector<float> & R_ij)const=0;

protected:
    float _maxDist;
    float _sqrMaxDist;
};

#endif // SPHKERNEL_H
