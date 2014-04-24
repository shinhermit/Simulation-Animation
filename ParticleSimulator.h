#ifndef PARTICLESIMULATOR_H
#define PARTICLESIMULATOR_H

#include <qclcontext.h>

#include "Simulator.h"
#include "SPHKernels.h"
#include "Particle.h"
#include "DefaultParameters.h"

class ParticleSimulator : public Simulator
{
    Q_OBJECT

public:
    ParticleSimulator(int debug=0, QGLViewer *viewer=NULL, wlMesh *environment=NULL);

    /// \brief Fournit le context pour un calcul en GPU et active le mode de calcul parallèle en GPU
    virtual void setOpenClContext(QCLContext * _openClContext=NULL, QCLVector<float> * _openClInput=NULL)
        throw(std::runtime_error);

    virtual void createParticles(const unsigned int & nbItems, const int & debug);

public slots:
    /// \brief Active/désactive le mode de calcul parallèle en GPU. Exception si activation et context GPU non défini (setOpenClContext).
    bool isGPUMode()const;

    /// \brief Définit la distance maximale d'influence entre les particules de la simulation
    float getPressureTolerance()const;

    /// \brief Définit le coefficient de dépression
    float getPressureToDensityGradientProportionnality()const;

    /// \brief Définit le coefficient de viscosité dynamique
    float getDynamicViscosityConstant()const;

    /// \brief Définit le coefficient de viscosité dynamique
    float getInitialDensity()const;

    /// \brief Active/désactive le mode de calcul parallèle en GPU. Exception si activation et context GPU non défini (setOpenClContext).
    virtual void setGPUMode(const bool & gpuMode) throw(std::logic_error);

    /// \brief Définit la distance maximale d'influence entre les particules de la simulation
    void setSmoothingTolerance(const double & coeff_d) throw(std::invalid_argument);

    /// \brief Définit le coefficient de dépression
    void setPressureToDensityGradientProportionnality(const double & coeff_k);

    /// \brief Définit le coefficient de viscosité dynamique
    void setDynamicViscosityConstant(const double & coeff_mu);

    /// \brief Définit le coefficient de viscosité dynamique
    void setInitialDensity(const double & coeff_rho0) throw(std::invalid_argument);

    /// \brief Définit la masse des particules de la simulation
    void setParticlesMass(const double & mass) throw(std::invalid_argument);

    virtual void reset();

    /// \brief Execute un pas de temps de l'animation.
    virtual void step();

    /// Affiche la valeur des principales variables.
    virtual void printSelf();

protected:
    QVector<Particle> _particles;

    //*********** OpenCl *********
    bool _gpuMode;

    // The following 2 pointers are received from class Project
    // No need for memory cleaning
    QCLContext * _openClContext;
    QCLVector<float> * _openClInput;

    QCLProgram _openClProgram;
    QCLKernel _openClDensityKernel;
    QCLKernel _openClTranslationKernel;

    //***** Constantes (paramètres) des équations de Navier-Stokes ******
    // Distance maximale d'influence entre deux particules
    float _coeff_d;
    //Coefficient de proportionnalité pression/gradient de densité
    float _coeff_k;
    //Coefficient de viscosité dynamique (résistance à la déformation)
    float _coeff_mu;
    //Densité de référence
    float _coeff_rho0;

    // vide toutes les structures internes
    virtual void _clear();

private:
    void _cpuStep();
    void _gpuStep();
};

#endif // PARTICLESIMULATOR_H
