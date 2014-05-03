#ifndef PROJECT_H
#define PROJECT_H

#include "ProjectView.h"
#include "DefaultParameters.h"

class Project
{
private:
    ProjectView * _view; /*!< The view of the project */
    ParticleSimulator * _simulator; /*!< The simulator */

    QCLContext _openClContext; /*!< Creates the openCL context */

    // Vectors that will hold particle dynamic properties
    // Each particle is represented by 8 contiguous values
    // <px,py,pz, vx,vy,vz, density,pressure>
    QCLVector<float> _openClInput; /*!< Holds an input vector created by opencContext */
    QCLVector<float> _openClOutput;/*!< Holds an input vector created by opencContext */

    /// \brief Sets the openCL context up
    void _configOpenCL(const unsigned int & nbItems);
    /// \brief Set the view up
    void _setView();
    /// \brief Instanciates a particle simulator
    /// \param nbItems number of particles of simulator
    /// \param viewer the viewer the simulator will bound to
    void _setSimulator(const unsigned int & nbItems, QGLViewer *viewer);

public:
    /// \param nbItems number of particle in the simulation
    Project(const unsigned int & nbItems=DefaultParameters::NbParticles);
    ~Project();

    /// \brief Set the computation mode to GPU
    void setGPUMode(const bool & trueFalse);
    /// \brief Tells wether or not the GPU mode is active
    bool isGPUMode()const;

    /// \brief Defines the size of the view
    /// Need to be fixed if enough time
    void setViewSize(const int & width, const int & height);
    /// \brief Display the entire project
    void show();
};

#endif // PROJECT_H
