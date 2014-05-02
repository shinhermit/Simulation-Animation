#include "Project.h"

Project::Project(const unsigned int & nbItems)
{
    _configOpenCL(nbItems);
    _setView();
    _setSimulator(nbItems, _view->getGLViewer());

    _view->bindSimulator(*_simulator);
    _view->update();
}

Project::~Project()
{
    _view->deleteLater();
    delete(_simulator);
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

void Project::_configOpenCL(const unsigned int & nbItems)
{
    _gpuMode = DefaultParameters::GpuMode;
     //OpenCL: size of kinematics values array
    unsigned int kinSize = DefaultParameters::OCLOffset * nbItems;

    if (!_openClContext.create())
    {
        std::cerr << "Could not create OpenCL context for the GPU\n" << std::endl;
        throw std::runtime_error("Project::_configOpenCL: Could not create OpenCL context for the GPU\n");
    }

    _openClInput = _openClContext.createVector<float>(kinSize);
    _openClOutput = _openClContext.createVector<float>(kinSize);
}

void Project::_setView()
{
    _view = new ProjectView();
    _view->move(0,0);
    _view->show();
}

void Project::_setSimulator(const unsigned int & nbItems, QGLViewer *viewer)
{
    _simulator = new ParticleSimulator(viewer);
    _simulator->setSmoothingTolerance(DefaultParameters::Coeff_d);
    _simulator->setPressureToDensityGradientProportionnality(DefaultParameters::Coeff_k);
    _simulator->setOpenClContext(nbItems, &_openClContext, &_openClInput, &_openClOutput);
    _simulator->createParticles(nbItems);
}
