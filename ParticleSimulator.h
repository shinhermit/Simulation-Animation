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

    /// \brief Sets a context for GPU computation and activate GPU mode.
    virtual void setOpenClContext(QCLContext * openClContext=NULL,
                                  QCLVector<float> * openClInput=NULL,
                                  QCLVector<float> * openClOutput=NULL) throw(std::runtime_error);
    /// \brief Creates the particules handled by this simulator.
    virtual void createParticles(const unsigned int & nbItems, const int & debug);

public slots:
    /// \brief Tells if GPU computation is active.
    bool isGPUMode()const;
    /// \brief Returns the maximale influence distance between particles.
    float getPressureTolerance()const;
    /// \brief Returns the proportionnality coefficient between pressure and density.
    float getPressureToDensityGradientProportionnality()const;
    /// \brief Returns the dynamic viscosity constant.
    float getDynamicViscosityConstant()const;
    /// \brief Returns the reference density (environment density).
    float getReferenceDensity()const;

    /// \brief Debug tool: prints particle properties to std::err
    void printParticles()const;
    /// \brief Debug tool: prints the contents of openclVector to std::err
    void printCLVectors()const;

    /// \brief Activates/desactivates the GPU computation. Exception thrown if activation attempt when no context set.
    virtual void setGPUMode(const bool & gpuMode) throw(std::logic_error);
    /// \brief Defines the maximale influence distance between particles.
    void setSmoothingTolerance(const double & coeff_d) throw(std::invalid_argument);
    /// \brief Defines the proportionnality coefficient between pressure and density.
    void setPressureToDensityGradientProportionnality(const double & coeff_k);
    /// \brief Defines the dynamic viscosity constant.
    void setDynamicViscosityConstant(const double & coeff_mu);
    /// \brief Defines the reference density (environment density).
    void setReferenceDensity(const double & coeff_rho0) throw(std::invalid_argument);
    /// \brief Defines the mass of all the particles in the simulation.
    void setParticlesMass(const double & mass) throw(std::invalid_argument);

    /// \reimp
    virtual void reset();
    /// \reimp
    virtual void step();

    /// \reimp
    virtual void printSelf();

protected:
    QVector<Particle> _particles;

    //*********** OpenCl *********
    bool _gpuMode; /*!< Tells if GPU computation is activated */

    // The following 3 pointers are received from class Project
    // No need for memory cleaning
    QCLContext * _openClContext; /*!< Holds the opencl context */
    QCLVector<float> * _openClInput; /*!< Holds the GPU inputs */
    QCLVector<float> * _openClOutput; /*!< Holds the GPU outputs */

    QCLProgram _openClProgram; /*!< The opencl program */
    QCLKernel _openClDensityKernel; /*!< Kernel for densities computation in GPU */
    QCLKernel _openClTranslationKernel; /*!< Kernel for positions computation in GPU */

    //***** Parameters for Navier-Stokes equations ******
    float _coeff_d; /*!< Maximal influence distance between particles */
    float _coeff_k; /*!< Pressure to density proportionality */
    float _coeff_mu; /*!< Dynamic viscosity */
    float _coeff_rho0; /*!< Reference density (environement density) */

    /// \reimp
    virtual void _clear();

private:
    /// \brief Compute one simulation step on CPU
    void _cpuStep();
    /// \brief Compute one simulation step on GPU
    void _gpuStep();
    /// \brief Copies the output of GPU computation to particles
    void _copyResults(const QCLVector<float> & openClOutput);
};

#endif // PARTICLESIMULATOR_H
