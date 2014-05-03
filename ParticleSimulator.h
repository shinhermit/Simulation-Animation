#ifndef PARTICLESIMULATOR_H
#define PARTICLESIMULATOR_H

#include <qclcontext.h>
#include <ctime>
#include <QObject>
#include <qglviewer.h>

#include "Environment.h"

#include "SPHKernels.h"
#include "Particle.h"
#include "DefaultParameters.h"

/// Notes:
/// . we observed that trying to use QCLVector when in CPU memory space causes segfaults.
/// This observation came out because we tried to use only QCLVector, even when computing in CPU,
/// thus avoiding vector copies. We finally used QVector for CPU computing.
/// . When the number of particle is not very high, the random positionment of particle doesn't work fine.
/// We added the messages to slow it down.
class ParticleSimulator : public QObject
{
    Q_OBJECT

public:
    ParticleSimulator(QGLViewer *viewer=NULL);
    ~ParticleSimulator();

    /// \brief Sets a context for GPU computation and activate GPU mode.
    virtual void initialize(const int & workSize) throw(std::runtime_error);
    /// \brief Creates the particules handled by this simulator.
    virtual void createParticles();

signals:
    /// \brief Informs the view that the smoothing distance has been programatically changed.
    void smoothingDistanceChanged(double);
    /// \brief Signal to request an update of opengl display.
    void requestUpdateGL();

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
    /// \brief Returns the number of particle for this simulator
    int size()const;
    /// \brief Returns the size of the clVectors
    int clVectorsSize()const;

    /// \brief Debug tool: prints particle properties to std::err
    void printParticles();
    /// \brief Debug tool: prints the contents of openclVector to std::err
    void printCLVectors();

    /// \brief Defines the number steps of the simulation.
    void setNumberOfTimeSteps(const int & nbSteps);
    /// \brief Defines the duration of a simulation step.
    void setTimeStep(const double & timestep);
    /// \brief Activates/desactivates the GPU computation. Exception thrown if activation attempt when no context set.
    virtual void setGPUMode(const bool & yesNo) throw(std::logic_error);
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
    virtual void restart();
    /// \reimp
    virtual void reset();
    /// \reimp
    virtual void step();
    /// \brief Computes several steps of the simulation.
    /// start a timer that will periodically compute one step, until the defined simulation's number of step is reeached.
    virtual void play();
    /// \brief Stops a previously played simulation.
    virtual void stop();

    /// \brief Draws all objects.
    virtual void draw();

    /// \reimp
    virtual void printSelf();

protected:
    Environment _env; /*!< Cubic limits of the scene */
    QGLViewer *_viewer; /*!< The opengl viewer */
    float _timestep; /*!< The duration of one step */
    int _nsteps; /*!< The number of steps of the simulation */
    int _cstep; /*!< The steps counter */
    QTimer * _timer; /*!< Timer, for simulation playing */
    bool _first; /*!< Tells if the entire scene has already been set up */

    // Vectors that will hold particle dynamic properties
    // Each particle is represented by 8 contiguous values
    //   <px,py,pz, vx,vy,vz, density,pressure>
    QVector<float> _initial; /*!< Initial kinematic properties, allowing scene reset */
    QVector<float> _input; /*!< Dynamic properties of the particles as input values */
    QVector<float> _output;  /*!< Dynamic properties of the particles as output values */
    QVector<float> * _input_p; /*!< Used to easily swap input and output vectors */
    QVector<float> * _output_p;  /*!< Used to easily swap input and output vectors */

    //*********** OpenCl *********
    bool _gpuMode; /*!< Tells if GPU computation is activated */

    QCLContext _clContext; /*!< Holds the opencl context */
    QCLProgram _clProgram; /*!< The opencl program */
    QCLKernel _clKernel; /*!< Kernel for positions computation in GPU */

    // The structure of the following clVectors is the same as the coresponding QVectors above
    QCLVector<float> _clInput; /*!< Holds the GPU inputs */
    QCLVector<float> _clOutput; /*!< Holds the GPU outputs */
    QCLVector<float> * _clInput_p; /*!< Used to swap input and output */
    QCLVector<float> * _clOutput_p; /*!< Used to swap input and output */

    //***** Parameters for Navier-Stokes equations ******
    float _coeff_d; /*!< Maximal influence distance between particles */
    float _coeff_k; /*!< Pressure to density proportionality */
    float _coeff_mu; /*!< Dynamic viscosity */
    float _coeff_rho0; /*!< Reference density (environement density) */
    float _particleMass; /*!< Uniform particles mass */

    /// \reimp
    virtual void _clear();

private:
    /// \brief CPU computation of the densities for all particles.
    void _computeDensities();
    /// \brief Use by _computePositions() to compute influences of pressure and viscosity
    /// \param i indice of the particle on which the influences are computed
    void _computeInfluences(const int & i, QVector<float> & gradPressure, QVector<float> & speedLaplacian);
    /// \brief Use by _computePositions() to compute kinematic properties base on influences computed by _computeInfluences()
    void _computeSmoothing(const int & i, const QVector<float> & gradPressure, const QVector<float> & speedLaplacian);
    /// \brief CPU computation of the positions for all particles.
    void _computePositions();
    /// \brief Compute one simulation step on CPU
    void _cpuStep();
    /// \brief Compute one simulation step on GPU
    void _gpuStep();
    /// \brief Dynamically computes the smoothing distance
    void _computeSmoothingDistance();
    /// \brief Swaps the input an output vectors for the next step
    void _swapVectors();
    /// \brief Sets the constant values that are to be passed to GPU kernel
    void _setKernelArgs(QCLKernel & kernel);
    /// \brief Copies the results of GPU computation from CLVector to QVector
    void _fetchResults();
    /// \brief Copies the results of CPU computation from QVector to CLVector
    void _updateCLInput();


    /// \brief Sets the scene up.
    /// Ensures that all objects are visible.
    virtual void _setupScene();
};

#endif // PARTICLESIMULATOR_H
