///////////////////////////////////////////////////////////////////////////////
//  File         :    wlSimulator.cxx                                        //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Simulateur pour le projet de SA                        //
//  Related file :    wlSimulator.h                                          //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#include "wlSimulator.h"

wlSimulator::wlSimulator(int debug, saViewer *glViewer,
                         wlSimulationEnvironment *env,
                         QVector<wlAnimatedMesh *> * objects)
    : wlCore(debug),
      environment(env),
      items(*objects),
      viewer(glViewer)
{
    this->Trace("-> wlSimulator(%p, %d)", environment, items.size());
    this->Clear();
    if (this->HasEnvironment())
        this->environment->SetViewer(viewer);
    for (int i=0 ; i<this->items.size() ; i++)
        this->items[i]->SetViewer(viewer);
    if (viewer != NULL)
        connect((QObject *)viewer, SIGNAL(drawNeeded()), this, SLOT(draw()));
    this->timer = new QTimer(this);
    this->timer->setInterval((int)(this->timestep*1000));
    connect(timer, SIGNAL(timeout()), this, SLOT(Step()));

    this->Trace("<- wlSimulator()");
}

wlSimulator::~wlSimulator()
{}


char * wlSimulator::GetClassName() const
{
    return "wlSimulator";
}

void
wlSimulator::Clear()
{
    this->nsteps = DefaultParameters::NbSteps;
    this->timestep = this->ctimestep = DefaultParameters::TimeStep;
    this->cstep = 0;
}

void
wlSimulator::PrintSelf()
{
    this->Print("Simulation en cours :");
    this->Print("  Pas de temps : %lf", this->timestep);
    this->Print("  Nombre de pas de temps : %d", this->nsteps);
    this->Print("  Pas de temps courant : %d", this->cstep);
    this->Print("  Dure du dernier pas de temps effectue : %lf", this->ctimestep);
    this->Print("Les objets animes :");
    for (int i=0 ; i<this->items.size() ; i++) {
        this->Print("  Objet %d :", i);
        this->items[i]->PrintSelf();
    }
}

void
wlSimulator::PrintContent()
{
    this->Print("Simulation en cours :");
    this->Print("  Pas de temps : %lf", this->timestep);
    this->Print("  Nombre de pas de temps : %d", this->nsteps);
    this->Print("  Pas de temps courant : %d", this->cstep);
    this->Print("  Dure du dernier pas de temps effectue : %lf", this->ctimestep);
    this->Print("Les objets animes :");
    for (int i=0 ; i<this->items.size() ; i++) {
        this->Print("  Objet %d :", i);
        this->items[i]->PrintContent();
    }
}

void
wlSimulator::SetEnvironment(wlSimulationEnvironment *env)
{
    if (env == NULL)
        return;
    if (this->HasEnvironment())
        delete this->environment;
    this->environment = env;
}

void
wlSimulator::AddItem(wlAnimatedMesh *item)
{
    if (item == NULL)
        return;
    item->SetViewer(this->viewer);
    this->items << item;
}

void
wlSimulator::ClearItems()
{
    this->items.clear();
}

bool wlSimulator::HasEnvironment()const
{
    return this->environment != NULL;
}

void
wlSimulator::SetTimeStep(QString t)
{
    bool ok;
    double e = t.toDouble(&ok);
    if (ok)
        this->SetTimeStep(e);
}

void
wlSimulator::SetTimeStep(double t)
{
    if (t > 0) {
        this->timestep = this->ctimestep = t;
        this->timer->setInterval((int)(t*1000));
        for (int i=0 ; i<this->items.size() ; i++)
            this->items[i]->SetTimeStep(t);
    }
}

void
wlSimulator::SetNumberOfTimeSteps(QString t)
{
    bool ok;
    double e = t.toInt(&ok);
    if (ok)
        this->SetNumberOfTimeSteps(e);
}

void
wlSimulator::SetNumberOfTimeSteps(int t)
{
    if (t > 0) {
        this->nsteps = t;
    }
}

void
wlSimulator::Reset()
{
    Clear();

    for(int i = 0; i < items.size(); ++i)
    {
        items[i]->Reset();
    }
}

void wlSimulator::Restart()
{
    for(int i = 0; i < items.size(); ++i)
    {
        items[i]->Reset();
    }
}

void
wlSimulator::Step()
{
    this->Trace("-> Step()");

    for(int i = 0; i < items.size(); ++i)
    {
        items[i]->Step();
    }

    ++this->cstep;

    if(this->cstep > this->nsteps)
        this->timer->stop();

    // maintenant qu'on est est parvenu a calculer un pas de temps on met a jour l'affichage
    this->viewer->updateGL();
    this->Trace("<- Step()");
}

void
wlSimulator::Play()
{
    this->Trace("-> Play() %d %d", this->cstep, this->nsteps);

    this->timer->start();

    this->Trace("<- Play()");
}

void
wlSimulator::Stop()
{
    this->Trace("-> Stop()");

    this->timer->stop();

    this->Trace("<- Stop()");
}

void
wlSimulator::ShowEntireMesh()
{
    std::vector<float> ext;
    if (this->HasEnvironment())
        ext = this->environment->GetExtends();
    else {
        ext.reserve(6);
        ext[0] = ext[2] = ext[4] = FLT_MAX;
        ext[1] = ext[3] = ext[5] = -FLT_MAX;
    }
    for (int i=0 ; i<this->items.size() ; i++) {
        std::vector<float> ext1 = this->items[i]->GetExtends();
        for (int j=0 ; j<5 ; j+=2) {
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

    this->viewer->setSceneRadius(radius*2);
    this->viewer->setSceneCenter(qglviewer::Vec(center[0], center[1], 0));
    this->viewer->camera()->setRevolveAroundPoint(this->viewer->sceneCenter());
    // on se place en [0, -y, 0]
    this->viewer->camera()->setPosition(qglviewer::Vec(center[0], center[1]-radius*10, center[2]));
    this->viewer->camera()->setViewDirection(this->viewer->sceneCenter() - this->viewer->camera()->position());
    this->viewer->camera()->setUpVector(qglviewer::Vec(0, 0, 1));
    this->viewer->camera()->setFieldOfView((float)atan((double)0.2));
}

void
wlSimulator::draw()
{
    static int first=1;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (this->HasEnvironment()) {
        glPushMatrix();
        glCallList(this->environment->GetList());
        glPopMatrix();
    }
    for (int i=0 ; i<this->items.size() ; i++) {
        glPushMatrix();
        glCallList(this->items[i]->GetList());
        glPopMatrix();
    }
    glEnable(GL_LIGHT0);
    float light_position[] = { 0, -100, 100, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    if (first) {
        this->ShowEntireMesh();
        first = 0;
    }
    glFlush();
}
