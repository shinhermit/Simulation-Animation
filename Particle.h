#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdexcept>

#include "wlAnimatedSphere.h"
#include "SPHKernels.h"
#include "DefaultParameters.h"

class Particle : public wlAnimatedMesh
{
public:
    Particle(const QVector<wlAnimatedMesh *> & everyone,
             int debug=0, wlQGLViewer *const v=NULL, QString filename=QString());

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
    virtual void Reset();
    /// \brief On redéfinit cette méthode pour la désactiver.
    virtual void Step();
    /// \brief Revient au pas de temps precedent.
    virtual void Back();

    virtual void computeDensity(const SPHKernel & kernel, const float & refDensity, const float & coeff_k);

    virtual void computeTranslation(const SPHKernel & kernelP, const SPHKernel & kernelV, const float & coeff_mu);

protected:
    float _mass;
    float _density;
    float _pressure;

    float _previous_density;
    float _previous_pressure;

    const QVector<wlAnimatedMesh *> & _everyone;

private:
    virtual void Clear();

    void _displayBefore()const;
    void _displayAfter(const QVector<float> & acc)const;
};

#endif // PARTICLE_H
