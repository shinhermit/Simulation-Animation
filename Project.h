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
    // 6 valeurs contigues <px,py,pz,vx,vy,vz>
    QCLVector<float> _openClInput;

    void _configOpenCL(const bool & gpuMode, const unsigned int & nbItems);
    void _setView();
    void _setSimulator(const unsigned int & nbItems, saViewer *viewer, int debug);

public:
    Project(const unsigned int & nbItems=DefaultParameters::NbParticles,
            const int & debug=0, const bool & gpuMode=false);
    ~Project();
    virtual char *GetClassName();

    void setViewSize(const int & width, const int & height);
    void show();
};

#endif // PROJECT_H
