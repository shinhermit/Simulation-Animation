#include "Simulator.h"

Simulator::Simulator(QGLViewer *viewer, QVector<AnimatedObject*> * items)
    : _items(*items),
      _viewer(viewer)
{
    _clear();

    if (_viewer != NULL)
        connect(_viewer, SIGNAL(drawNeeded()), this, SLOT(draw()));

    _timer = new QTimer(this);
    _timer->setInterval((int)(_timestep*1000));
    connect(_timer, SIGNAL(timeout()), this, SLOT(step()));
}

Simulator::~Simulator()
{}


char * Simulator::getClassName() const
{
    return "Simulator";
}

void Simulator::_clear()
{
    _nsteps = DefaultParameters::NbSteps;
    _timestep = DefaultParameters::TimeStep;
    _cstep = 0;
}

void Simulator::printSelf()
{
    ::fprintf(stderr, "Simulation en cours :");
    ::fprintf(stderr, "  Pas de temps : %lf", _timestep);
    ::fprintf(stderr, "  Nombre de pas de temps : %d", _nsteps);
    ::fprintf(stderr, "  Pas de temps courant : %d", _cstep);

    ::fprintf(stderr, "Les objets animes :");
    for (int i=0 ; i<_items.size() ; i++)
    {
        ::fprintf(stderr, "  Objet %d :", i);
        _items[i]->printSelf();
    }
}

void Simulator::addItem(AnimatedObject * item)
{
    if (item != NULL)
    {
        _items << item;
    }
}

void Simulator::clearItems()
{
    _items.clear();
}

void Simulator::setTimeStep(const double & timestep)
{
    if (timestep > 0)
    {
        _timestep = timestep;
        _timer->setInterval((int)(timestep*1000));

        for (int i=0 ; i<_items.size() ; i++)
            _items[i]->setTimeStep(timestep);
    }
}

void Simulator::setNumberOfTimeSteps(const int & nbSteps)
{
    if (nbSteps > 0)
    {
        _nsteps = nbSteps;
    }
}

void Simulator::reset()
{
    _nsteps = DefaultParameters::NbSteps;
    _timestep = DefaultParameters::TimeStep;
    _cstep = 0;

    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->reset();
    }

    this->draw();
    emit requestUpdateGL();
}

void Simulator::restart()
{
    _cstep = 0;

    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->reset();
    }

    this->draw();
    emit requestUpdateGL();
}

void Simulator::step()
{
    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->step();
    }

    ++_cstep;

    if(_cstep > _nsteps)
        _timer->stop();

    this->draw();
    emit requestUpdateGL();
}

void Simulator::play()
{
    _timer->start();
}

void Simulator::stop()
{
    _timer->stop();
}

void Simulator::_setupScene()
{
    QVector<float> ext;
    std::vector<float> size;
    std::vector<float> center;

    ext = _env.getLimits();

    size.push_back(ext[1] - ext[0]);
    size.push_back(ext[3] - ext[2]);
    size.push_back(ext[5] - ext[4]);

    center.push_back(ext[0] + size[0] / 2);
    center.push_back(ext[2] + size[1] / 2);
    center.push_back(ext[4] + size[2] / 2);

    float radius = (float)sqrt((double)(size[0] * size[0] + size[1] * size[1] + size[2] * size[2])) / 2;

    _viewer->setSceneRadius(radius*2);
    _viewer->setSceneCenter(qglviewer::Vec(center[0], center[1], 0));
    _viewer->camera()->setRevolveAroundPoint(_viewer->sceneCenter());

    // on se place en [0, -y, 0]
    _viewer->camera()->setPosition(qglviewer::Vec(center[0], center[1]-radius*10, center[2]));
    _viewer->camera()->setViewDirection(_viewer->sceneCenter() - _viewer->camera()->position());
    _viewer->camera()->setUpVector(qglviewer::Vec(0, 0, 1));
    _viewer->camera()->setFieldOfView((float)atan((double)0.2));
    _viewer->camera()->showEntireScene();
}

void Simulator::draw()
{
    static int first=1;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _env.draw();

    glPushMatrix();
    glColor3f(0.6, 0.6, 0.6);
    glPointSize(2.);
    glBegin(GL_POINTS);
    for (int i=0 ; i<_items.size() ; ++i)
    {
        QVector<float> p = _items[i]->getPosition();
        glVertex3f(p[0], p[1], p[2]);
    }
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHT0);
    float light_position[] = { 0, -100, 100, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if (first)
    {
        _setupScene();
        first = 0;
    }

    glFlush();
}
