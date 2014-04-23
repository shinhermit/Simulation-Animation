#include "Project.h"

Project::Project(const unsigned int & nbItems,
                 const int & debug, const bool & gpuMode)
{
    this->Trace("-> Project()");

    _configOpenCL(gpuMode, nbItems);
    _setView();
    _setSimulator(nbItems, _view->getGLViewer(), debug);

    _view->bindSimulator(*_simulator);

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

void Project::show()
{
    _view->show();
}

void Project::_configOpenCL(const bool & gpuMode, const unsigned int & nbItems)
{
    _gpuMode = gpuMode;
    unsigned int kinSize = 6*nbItems; //OpenCL: taille du tableau des valeurs cinématiques

    if(_gpuMode)
    {
        if (!this->_openClContext.create())
        {
            std::cerr << "Could not create OpenCL context for the GPU\n" << std::endl;
            throw std::runtime_error("saProject::saProject: Could not create OpenCL context for the GPU\n");
        }

        this->_openClInput = this->_openClContext.createVector<float>(kinSize);
    }
}

void Project::_setView()
{
    _view = new ProjectView();
    _view->move(0,0);
    _view->show();
}

void Project::_setSimulator(const unsigned int & nbItems, saViewer *viewer, int debug)
{
    wlSimulationEnvironment *env = new wlGround(debug);

    _simulator = new ParticleSimulator(nbItems, debug, viewer, env);
    _simulator->setSmoothingTolerance(10.);
    _simulator->setPressureToDensityGradientProportionnality(20.);

    if(this->_gpuMode)
    {
        _simulator->setOpenClContext(&this->_openClContext, &this->_openClInput);
    }

    _simulator->draw();
}
