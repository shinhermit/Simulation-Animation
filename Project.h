#ifndef PROJECT_H
#define PROJECT_H

#include "wlCore.h"
#include "wlGround.h"
#include "ProjectView.h"
#include "DefaultParameters.h"

class Project : public wlCore
{
private:
    ProjectView * _view;
    ParticleSimulator * _simulator;

    //*********** OpenCl *********
    // Switch to enable GPU computing
    bool _gpuMode;

    // Contexte GPU
    QCLContext _openClContext;

    //Pour transmettre les données au GPU
    // Vecteur de positions et vitesses
    // chaque particule est représentée par
    // 8 valeurs contigues <px,py,pz, vx,vy,vz, density,pressure>
    QCLVector<float> _openClInput;
    QCLVector<float> _openClOutput;

    void _configOpenCL(const bool & gpuMode, const unsigned int & nbItems);
    void _setView();
    void _setSimulator(const unsigned int & nbItems, QGLViewer *viewer, const int & debug);

public:
    Project(const unsigned int & nbItems=DefaultParameters::NbParticles,
            const int & debug=0, const bool & gpuMode=false);
    ~Project();
    virtual char *GetClassName();

    void setGPUMode(const bool & trueFalse);
    bool isGPUMode()const;

    void setViewSize(const int & width, const int & height);
    void show();
};

#endif // PROJECT_H
