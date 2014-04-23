///////////////////////////////////////////////////////////////////////////////
//  File         :    wlAnimatedMesh.cxx                                     //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Modele de mesh animee                                  //
//  Related file :    wlAnimatedMesh.h, wlMesh.{h,cxx}                       //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#include <wlAnimatedMesh.h>

wlAnimatedMesh::wlAnimatedMesh(int debug, wlQGLViewer *v, QString filename)
    : wlMesh(debug, v, filename)
{
    this->Trace("-> wlAnimatedMesh(%p, \"%s\")", v, filename.toAscii().constData());

    this->Tmat << 0 << 0 << 0;

    this->Clear();

    this->Trace("<- wlAnimatedMesh()");
}

void
wlAnimatedMesh::Clear()
{
    this->velocity.clear();
    this->velocity << 0.0 << 0.0 << 0.0;
    this->timestep = 0.1;
    this->cstep = 0;
    this->cvel = this->velocity;
    this->position = this->GetCenter();
    this->Tmat[0] = this->Tmat[1] = this->Tmat[2] = 0;

    this->Modified("Position");
}

void
wlAnimatedMesh::PrintSelf()
{
    QVector<double> pos = getPosition();

    this->Print("position initiale : %.2lf %.2lf %.2lf", this->position[0], this->position[1], this->position[2]);
    this->Print("vitesse initiale : %.2lf %.2lf %.2lf", this->velocity[0], this->velocity[1], this->velocity[2]);
    this->Print("position actuelle : %.2f %.2f %.2f", pos[0], pos[1], pos[2]);

    wlMesh::PrintSelf();
}

void
wlAnimatedMesh::PrintContent()
{
    this->PrintSelf();
}

QVector<double> wlAnimatedMesh::getPosition() const
{
    QVector<double> pos;
    pos << this->position[0] << this->position[1] << this->position[2];

    return pos + this->Tmat;
}

void
wlAnimatedMesh::SetPosition(double Px, double Py, double Pz)
{
    if (Px != this->position[0] || Py != this->position[0] || Pz != this->position[0])
    {
        QVector<float> v = QVector<float>::fromStdVector(this->position);
        v[0] = Px - v[0]; v[1] = Py - v[1]; v[2] = Pz - v[2];

        this->Translate(v);

        this->position[0] = Px;
        this->position[1] = Py;
        this->position[2] = Pz;

        this->Tmat.fill(0);

        this->Modified("Position");
        this->Modified("DisplayList");

        if (this->HasViewer())
            this->viewer->updateGL();
    }
}

void
wlAnimatedMesh::SetPositionX(QString Px)
{
    bool ok;
    double e = Px.toDouble(&ok);
    if (ok)
        this->SetPosition(e, this->position[1], this->position[2]);
}

void
wlAnimatedMesh::SetPositionY(QString Py)
{
    bool ok;
    double e = Py.toDouble(&ok);
    if (ok)
        this->SetPosition(this->position[0], e, this->position[2]);
}

void
wlAnimatedMesh::SetPositionZ(QString Pz)
{
    bool ok;
    double e = Pz.toDouble(&ok);
    if (ok)
        this->SetPosition(this->position[0], this->position[1], e);
}

void
wlAnimatedMesh::SetVelocity(double Vx, double Vy, double Vz)
{
    if (Vx != this->velocity[0] || Vy != this->velocity[0] || Vz != this->velocity[0])
    {
        this->velocity[0] = Vx;
        this->velocity[1] = Vy;
        this->velocity[2] = Vz;
    }
}

void
wlAnimatedMesh::SetVelocityX(QString Vx)
{
    bool ok;
    double e = Vx.toDouble(&ok);
    if (ok)
        this->SetVelocity(e, this->velocity[1], this->velocity[2]);
}

void
wlAnimatedMesh::SetVelocityY(QString Vy)
{
    bool ok;
    double e = Vy.toDouble(&ok);
    if (ok)
        this->SetVelocity(this->velocity[0], e, this->velocity[2]);
}

void
wlAnimatedMesh::SetVelocityZ(QString Vz)
{
    bool ok;
    double e = Vz.toDouble(&ok);
    if (ok)
        this->SetVelocity(this->velocity[0], this->velocity[1], e);
}

void
wlAnimatedMesh::SetTimeStep(double t)
{
    if (t > 0)
    {
        this->timestep = t;
    }
}

void
wlAnimatedMesh::SetTimeStep(QString t)
{
    bool ok;
    double e = t.toDouble(&ok);
    if (ok)
        this->SetTimeStep(e);
}


void
wlAnimatedMesh::Reset()
{
    this->Trace("-> Reset()");
    this->Tmat[0] = this->Tmat[1] = this->Tmat[2] = 0;
    this->cvel = this->velocity;
    this->cstep = 0;

    this->Modified("Position");
    this->Modified("DisplayList");
    if (this->HasViewer())
        this->viewer->updateGL();
    this->Trace("<- Reset()");
}

void
wlAnimatedMesh::Step()
{
    this->Trace("-> Step()");

    this->Tmat[0] += this->cvel[0] * this->timestep;
    this->Tmat[1] += this->cvel[1] * this->timestep;
    this->Tmat[2] += (this->cvel[2] - G * this->timestep) / 2 * this->timestep;

    this->cvel[2] -= G * this->timestep;

    ++this->cstep;

    this->Modified("Position");
    this->Modified("DisplayList");
      this->Trace("<- Step()");
}

void
wlAnimatedMesh::glTransformations()
{
    glTranslated(this->Tmat[0], this->Tmat[1], this->Tmat[2]);
}

void
wlAnimatedMesh::makeList(int reset_camera)
{
    wlMesh::makeList(reset_camera);
}

void
wlAnimatedMesh::draw()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10, -50, 10, 10, 0, 10, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-3, 3, -3, 3, 10, 100);
    glMatrixMode(GL_MODELVIEW);
    glCallList(this->GetList());
    glEnable(GL_LIGHT0);
    float light_position[] = { 0, -this->GetRadius()*10, this->GetRadius()*10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glFlush();
}
