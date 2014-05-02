#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdexcept>
#include <iostream>

#include "AnimatedObject.h"
#include "SPHKernels.h"
#include "DefaultParameters.h"
#include "Environment.h"

class Particle : public AnimatedObject
{
public:
    Particle(const QVector<AnimatedObject*> & everyone, const Environment & env);

    /// \brief Defines the mass of the particle
    virtual void setMass(const float & mass) throw(std::invalid_argument);

    /// \brief Returns the mass of the particle
    virtual float getMass()const;
    /// \brief returns the density of the particle
    virtual float getDensity()const;
    /// \brief Returns the pressure on the particle
    virtual float getPressure()const;

public slots:
    /// \brief Defines the density of the particle. Useful for the update after a GPU computation
    virtual void setDensity(const float & density);
    /// \brief Defines the pressure of the particle. Useful for the update after a GPU computation
    virtual void setPressure(const float & pressure);
    /// \brief Computes the SPH for the density (and thus the pressure) of the particle
    virtual void computeDensity(const float & maxDist, const float & refDensity, const float & coeff_k);
    /// \brief Computes the SPH for position.
    virtual void computeTranslation(const float & maxDist, const float & coeff_mu);

    /// \reimp
    virtual void reset();
    /// \reimp
    virtual void step();

    /// \reimp
    virtual void printSelf()const;

protected:
    float _mass; /*!< The mass of the particle */
    float _density; /*!< The density of the particle */
    float _pressure; /*!< The pressure on the particle */

    const Environment & _env; /*!< Scene limits constraints */
    const QVector<AnimatedObject *> & _everyone; /*!< All the particle, for SPH computation */

    /// \brief Reinitializes all the properties
    virtual void _clear();
};

#endif // PARTICLE_H
