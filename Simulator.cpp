#include "Simulator.h"

Simulator::Simulator(int debug, QGLViewer *viewer,
                         wlMesh *env,
                         QVector<AnimatedObject*> * items)
    : wlCore(debug),
      _env(env),
      _items(*items),
      _viewer(viewer)
{
    this->Trace("-> Simulator(%p, %d)", _env, _items.size());

    _clear();

//    if (this->hasEnvironment())
//        _env->SetViewer(_viewer);

    if (_viewer != NULL)
        connect(_viewer, SIGNAL(drawNeeded()), this, SLOT(draw()));

    _timer = new QTimer(this);
    _timer->setInterval((int)(_timestep*1000));
    connect(_timer, SIGNAL(timeout()), this, SLOT(step()));

    this->Trace("<- Simulator()");
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
    this->Print("Simulation en cours :");
    this->Print("  Pas de temps : %lf", _timestep);
    this->Print("  Nombre de pas de temps : %d", _nsteps);
    this->Print("  Pas de temps courant : %d", _cstep);

    this->Print("Les objets animes :");
    for (int i=0 ; i<_items.size() ; i++)
    {
        this->Print("  Objet %d :", i);
        _items[i]->printSelf();
    }
}

void Simulator::setEnvironment(wlMesh *env)
{
    if (env != NULL)
    {
        if (this->hasEnvironment())
            delete _env;

        _env = env;
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

bool Simulator::hasEnvironment()const
{
    return _env != NULL;
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
    _clear();

    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->reset();
    }

    this->draw();
    emit requestUpdateGL();
}

void Simulator::restart()
{
    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->reset();
    }

    this->draw();
    emit requestUpdateGL();
}

void Simulator::step()
{
    this->Trace("-> step()");

    for(int i = 0; i < _items.size(); ++i)
    {
        _items[i]->step();
    }

    ++_cstep;

    if(_cstep > _nsteps)
        _timer->stop();

    this->Trace("<- step()");

    this->draw();
    emit requestUpdateGL();
}

void Simulator::play()
{
    this->Trace("-> play() %d %d", _cstep, _nsteps);

    _timer->start();

    this->Trace("<- play()");
}

void Simulator::stop()
{
    this->Trace("-> stop()");

    _timer->stop();

    this->Trace("<- stop()");
}

void Simulator::_showEntireMesh()
{
    std::vector<float> ext;
    if (this->hasEnvironment())
        ext = _env->GetExtends();

    else
    {
        ext.reserve(6);
        ext[0] = ext[2] = ext[4] = FLT_MAX;
        ext[1] = ext[3] = ext[5] = -FLT_MAX;
    }

    for (int i=0 ; i<_items.size() ; i++)
    {
        QVector<float> p = _items[i]->getPosition();
        QVector<float> ext1;
        ext1 << p[0] << p[0] << p[1] << p[1] << p[2] << p[2];

        for (int j=0 ; j<5 ; j+=2)
        {
            ext[j] = ext1[j] < ext[j] ? ext1[j] : ext[j];
            ext[j+1] = ext1[j+1] > ext[j+1] ? ext1[j+1] : ext[j+1];
        }
    }

    std::vector<float> size;
    size.push_back(ext[1] - ext[0]);
    size.push_back(ext[3] - ext[2]);
    size.push_back(ext[5] - ext[4]);

    std::vector<float> center;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    if (this->hasEnvironment())
//    {
//        glPushMatrix();
//        glCallList(_env->GetList());
//        glPopMatrix();
//    }
    glPushMatrix();
    glColor3f(0., 0.6, 0.3);
    glBegin(GL_TRIANGLES);
        glVertex3f(50., 50., 0);
        glVertex3f(-50., 50., 0);
        glVertex3f(-50., -50., 0);

        glVertex3f(-50., -50., 0);
        glVertex3f(50., -50., 0);
        glVertex3f(50., 50., 0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.6, 0.6, 0.6);
    glPointSize(1.);
    glBegin(GL_POINTS);
    for (int i=0 ; i<_items.size() ; ++i)
    {
        QVector<float> p = _items[i]->getPosition();
        //std::cerr << "Simulator::draw: drawing at potision (" << p[0] << "," << p[1] << "," << p[2] << ")" << std::endl;
        glVertex3f(p[0], p[1], p[2]);
    }
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHT0);
    float light_position[] = { 0, -100, 100, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if (first)
    {
        _showEntireMesh();
        first = 0;
    }

    glFlush();
}