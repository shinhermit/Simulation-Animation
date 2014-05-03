#ifndef PROJECT_H
#define PROJECT_H

#include "ProjectView.h"
#include "DefaultParameters.h"

class Project
{
private:
    ProjectView * _view;
    ParticleSimulator * _simulator;

    void _configOpenCL();
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
