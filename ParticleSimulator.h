#ifndef PARTICLESIMULATOR_H
#define PARTICLESIMULATOR_H

#include "wlSimulator.h"
#include "SPHKernels.h"
#include "Particle.h"

class ParticleSimulator : public wlSimulator
{
    Q_OBJECT

public:
    ParticleSimulator(const unsigned int & nbParticles=27, int debug=0, saViewer *viewer=NULL,
                      wlSimulationEnvironment *environment=NULL);

    /// \brief Fournit le context pour un calcul en GPU et active le mode de calcul parallèle en GPU
    virtual void setOpenClContext(QCLContext * _openClContext=NULL, QCLVector<float> * _openClInput=NULL);

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

    /// \brief Execute un pas de temps de l'animation.
    virtual void Step();

    /// Affiche la valeur des principales variables.
    virtual void PrintSelf();

protected:
    QVector<Particle> _particles;

    //*********** OpenCl *********
    bool _gpuMode;
    QCLContext * _openClContext;
    QCLVector<float> * _openClInput;
    QCLProgram _openClProgram;
    QCLKernel _openClKernel;

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
    virtual void Clear();

private:
    void _createParticles(const unsigned int & nbItems, const int & debug);
    void _cpuStep();
    void _gpuStep();
};

#endif // PARTICLESIMULATOR_H
