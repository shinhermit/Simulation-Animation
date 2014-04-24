#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdexcept>
#include <iostream>

#include "AnimatedObject.h"
#include "SPHKernels.h"
#include "DefaultParameters.h"

class Particle : public AnimatedObject
{
public:
    Particle(const QVector<AnimatedObject*> & everyone, int debug=0);

    /// \brief Definit la masse de la mesh.
    virtual void setMass(const float & mass) throw(std::invalid_argument);

    /// \brief Retourne la masse de la particule
    virtual float getMass()const;

    /// \brief Retourne la pression de la particule
    virtual float getDensity()const;

    /// \brief Retourne la pression de la particule
    virtual float getPressure()const;

public slots:
    /// \brief Ramene l'objet dans sa configuration initiale.
    virtual void reset();
    /// \brief On redéfinit cette méthode pour la désactiver.
    virtual void step();
    /// \brief Définit la densité de la particule. Utile pour la mise à jour après calcul en GPU
    virtual void setDendity(const float & density);
    /// \brief Définit la densité de la particule. Utile pour la mise à jour après calcul en GPU
    virtual void setPressure(const float & pressure);

    virtual void computeDensity(const SPHKernel & kernel, const float & refDensity, const float & coeff_k);

    virtual void computeTranslation(const SPHKernel & kernelP, const SPHKernel & kernelV, const float & coeff_mu);

protected:
    float _mass;
    float _density;
    float _pressure;

    const QVector<AnimatedObject *> & _everyone;

    virtual void _clear();

    void _displayBefore()const;
    void _displayAfter(const QVector<float> & acc)const;
};

#endif // PARTICLE_H
