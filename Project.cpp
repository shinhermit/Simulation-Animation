#include "Project.h"

Project::Project(const unsigned int & nbItems,
                 const int & debug, const bool & gpuMode)
{
    this->Trace("-> Project()");

    _configOpenCL(gpuMode, nbItems);
    _setView();
    _setSimulator(nbItems, _view->getGLViewer(), debug);

    _view->bindSimulator(*_simulator);
    _view->update();

    this->Trace("<- Project");
}

Project::~Project()
{
    _view->deleteLater();
    delete(_simulator);
}

char *Project::GetClassName()
{
    return "SA Project";
}

void Project::setGPUMode(const bool & trueFalse)
{
    _simulator->setGPUMode(trueFalse);
}

bool Project::isGPUMode() const
{
    return _simulator->isGPUMode();
}

void Project::setViewSize(const int & width, const int & height)
{
    _view->setSize(width, height);
}

void Project::show()
{
    _view->show();
}

void Project::_configOpenCL(const bool & gpuMode, const unsigned int & nbItems)
{
    _gpuMode = gpuMode;
     //OpenCL: taille du tableau des valeurs cinématiques
    unsigned int kinSize = DefaultParameters::OCLOffset * nbItems;

    if(_gpuMode)
    {
        if (!_openClContext.create())
        {
            std::cerr << "Could not create OpenCL context for the GPU\n" << std::endl;
            throw std::runtime_error("Project::_configOpenCL: Could not create OpenCL context for the GPU\n");
        }

        _openClInput = _openClContext.createVector<float>(kinSize);
        _openClOutput = _openClContext.createVector<float>(kinSize);
    }
}

void Project::_setView()
{
    _view = new ProjectView();
    _view->move(0,0);
    _view->show();
}

void Project::_setSimulator(const unsigned int & nbItems, QGLViewer *viewer, const int & debug)
{
    wlMesh * env = new wlGround(debug);

    _simulator = new ParticleSimulator(debug, viewer, env);
    _simulator->setSmoothingTolerance(DefaultParameters::Coeff_d);
    _simulator->setPressureToDensityGradientProportionnality(DefaultParameters::Coeff_k);

    if(_gpuMode)
    {
        _simulator->setOpenClContext(&_openClContext, &_openClInput, &_openClOutput);
    }

    _simulator->createParticles(nbItems, debug);
}
