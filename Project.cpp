#include "Project.h"

Project::Project(const unsigned int & nbItems)
{
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

void Project::_configOpenCL()
{
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
    _simulator->setOpenClContext(nbItems);
    _simulator->createParticles();
}
