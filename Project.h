#ifndef PROJECT_H
#define PROJECT_H

#include "ProjectView.h"
#include "DefaultParameters.h"

class Project
{
private:
    ProjectView * _view;
    ParticleSimulator * _simulator;

    QCLContext _openClContext;

    // Vectors that will hold particle dynamic properties
    // Each particle is represented by 8 contiguous values
    // <px,py,pz, vx,vy,vz, density,pressure>
    QCLVector<float> _openClInput;
    QCLVector<float> _openClOutput;

    void _configOpenCL(const unsigned int & nbItems);
    void _setView();
    void _setSimulator(const unsigned int & nbItems, QGLViewer *viewer);

public:
    Project(const unsigned int & nbItems=DefaultParameters::NbParticles);
    ~Project();

    void setGPUMode(const bool & trueFalse);
    bool isGPUMode()const;

    void setViewSize(const int & width, const int & height);
    void show();
};

#endif // PROJECT_H
