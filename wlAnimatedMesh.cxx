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
    this->timer = new QTimer(this);
    this->timer->setInterval((int)(this->timestep*1000));
    connect(timer, SIGNAL(timeout()), this, SLOT(Step()));
    this->Tmat << 0 << 0 << 0;
    this->Clear();
    this->Trace("<- wlAnimatedMesh()");
}

void
wlAnimatedMesh::PrintSelf()
{
    this->Print("position initiale : %.2lf %.2lf %.2lf", this->position[0], this->position[1], this->position[2]);
    this->Print("vitesse initiale : %.2lf %.2lf %.2lf", this->velocity[0], this->velocity[1], this->velocity[2]);
    this->Print("force instantanee : %.2lf %.2lf %.2lf", this->force[0], this->force[1], this->force[2]);
    this->Print("tolerance : %lf", this->tolerance);
    this->Print("position actuelle : %.2lf %.2lf %.2lf", this->GetCurrentCenter()[0], this->GetCurrentCenter()[1], this->GetCurrentCenter()[2]);
    wlMesh::PrintSelf();
}

void
wlAnimatedMesh::PrintContent()
{
    this->PrintSelf();
}

void
wlAnimatedMesh::Clear()
{
    this->force.clear();
    this->velocity.clear();
    this->force << 0.0 << 0.0 << 0.0;
    this->velocity << 0.0 << 0.0 << 0.0;
    this->reaction = PURE_KINEMATIC;
    this->k = this->Ks = this->m = 1.0;
    this->nsteps = 10;
    this->timestep = 0.1;
    this->cstep = 0;
    this->cvel = this->velocity;
    this->position = this->GetCenter();
    this->Tmat[0] = this->Tmat[1] = this->Tmat[2] = 0;
    this->tolerance = 0.01;
    this->ccenter.clear();
    this->ccenter << (double)this->position[0] << (double)this->position[1] << (double)this->position[2];

    this->Modified("Position");
}

QVector<double>
wlAnimatedMesh::GetCurrentCenter()
{
    if (!this->UpToDate("Position")) {
        this->ccenter[0] = (double)this->position[0];
        this->ccenter[1] = (double)this->position[1];
        this->ccenter[2] = (double)this->position[2];
        this->ccenter[0] += this->Tmat[0];
        this->ccenter[1] += this->Tmat[1];
        this->ccenter[2] += this->Tmat[2];
        this->Updated("Position");
    }
    return this->ccenter;
}

void
wlAnimatedMesh::SetPosition(double Px, double Py, double Pz)
{
    if (Px != this->position[0] || Py != this->position[0] || Pz != this->position[0]) {
        QVector<float> v = QVector<float>::fromStdVector(this->GetCenter());
        v[0] = Px - v[0]; v[1] = Py - v[1]; v[2] = Pz - v[2];
        this->Translate(v);
        this->position[0] = Px;
        this->position[1] = Py;
        this->position[2] = Pz;
        this->Tmat.fill(0);
        this->ccenter[0] = (double)this->position[0];
        this->ccenter[1] = (double)this->position[1];
        this->ccenter[2] = (double)this->position[2];
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
wlAnimatedMesh::SetForce(double Fx, double Fy, double Fz)
{
    if (Fx != this->force[0] || Fy != this->force[0] || Fz != this->force[0]) {
        this->force[0] = Fx;
        this->force[1] = Fy;
        this->force[2] = Fz;
    }
}

void
wlAnimatedMesh::SetForceX(QString Fx)
{
    bool ok;
    double e = Fx.toDouble(&ok);
    if (ok)
        this->SetForce(e, this->force[1], this->force[2]);
}

void
wlAnimatedMesh::SetForceY(QString Fy)
{
    bool ok;
    double e = Fy.toDouble(&ok);
    if (ok)
        this->SetForce(this->force[0], e, this->force[2]);
}

void
wlAnimatedMesh::SetForceZ(QString Fz)
{
    bool ok;
    double e = Fz.toDouble(&ok);
    if (ok)
        this->SetForce(this->force[0], this->force[1], e);
}

void
wlAnimatedMesh::SetVelocity(double Vx, double Vy, double Vz)
{
    if (Vx != this->velocity[0] || Vy != this->velocity[0] || Vz != this->velocity[0]) {
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
wlAnimatedMesh::SetReaction(int index)
{
    this->reaction = index;
}

void
wlAnimatedMesh::SetAttenuationCoefficientForPureKinematicReaction(QString t)
{
    bool ok;
    double e = t.toDouble(&ok);
    if (ok)
        this->SetAttenuationCoefficientForPureKinematicReaction(e);
}

void
wlAnimatedMesh::SetAttenuationCoefficientForPureKinematicReaction(double k)
{
    if (k != this->k && k >= 0 && k <= 1.0) {
        this->k = k;
        this->Modified("parameters");
    }
}

void
wlAnimatedMesh::SetSpringCoefficientForPenaltyReaction(QString t)
{
    bool ok;
    double e = t.toDouble(&ok);
    if (ok)
        this->SetSpringCoefficientForPenaltyReaction(e);
}

void
wlAnimatedMesh::SetSpringCoefficientForPenaltyReaction(double Ks)
{
    if (Ks != this->Ks && Ks >= 0) {
        this->Ks = Ks;
        this->Modified("parameters");
    }
}

void
wlAnimatedMesh::SetPointWeightForPenaltyReaction(QString t)
{
    bool ok;
    double e = t.toDouble(&ok);
    if (ok)
        this->SetPointWeightForPenaltyReaction(e);
}

void
wlAnimatedMesh::SetPointWeightForPenaltyReaction(double m)
{
    if (m != this->m && m >= 0) {
        this->m = m;
        this->Modified("parameters");
    }
}

void
wlAnimatedMesh::SetTimeStep(double t)
{
    if (t > 0) {
        this->timer->setInterval((int)(t*1000));
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
wlAnimatedMesh::SetNumberOfTimeSteps(QString t)
{
    bool ok;
    double e = t.toInt(&ok);
    if (ok)
        this->SetNumberOfTimeSteps(e);
}

void
wlAnimatedMesh::SetNumberOfTimeSteps(int t)
{
    this->nsteps = t>0 ? t : 1;
}

void
wlAnimatedMesh::Reset()
{
    this->Trace("-> Reset()");
    this->timer->stop();
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
    //  this->Trace("-> Step()");
    this->Tmat_p = this->Tmat;
    this->cvel_p = this->cvel;

    this->Tmat[0] += this->cvel[0] * this->timestep;
    this->Tmat[1] += this->cvel[1] * this->timestep;
    this->Tmat[2] += (this->cvel[2] - G * this->timestep) / 2 * this->timestep;

    this->cvel[2] -= G * this->timestep;

    if (this->cstep >= this->nsteps)
        this->timer->stop();

    ++this->cstep;

    this->Modified("Position");
    this->Modified("DisplayList");
    //  this->Trace("<- Step()");
}

void
wlAnimatedMesh::Back()
{
    if (this->cstep > 0)
    {
        --this->cstep;
        this->Tmat = this->Tmat_p;
        this->cvel = this->cvel_p;

        this->Modified("Position");
    }
}

void
wlAnimatedMesh::Play()
{
    this->Trace("-> Play()");

    if (this->cstep < this->nsteps)
        this->timer->start();

    this->Trace("<- Play()");
}

void
wlAnimatedMesh::Stop()
{
    this->Trace("-> Stop()");

    this->timer->stop();

    this->Trace("<- Stop()");
}

QVector<double>
wlAnimatedMesh::ComputeCollisionWithOther(wlAnimatedMesh *other)
{
    if (other == NULL)
        this->Error("ComputeCollision: pointeur sur autre mesh null");
    return QVector<double>();
}

QVector<double>
wlAnimatedMesh::ComputeCollisionWithPlan(QVector<double> equation)
{
    if (equation.size() != 6)
        this->Error("ComputeCollisionWithPlan: equation du plan invalide");
    return QVector<double>();
}

void
wlAnimatedMesh::ComputeReaction(QVector<double> impact)
{
    if (impact.size() < 6)
        this->Error("ComputeReaction: donnees sur l'impact incompletes");
}

QVector<double> wlAnimatedMesh::getPosition()const
{
    QVector<double> res;

    res << this->position[0] + this->Tmat[0]
        << this->position[1] + this->Tmat[1]
        << this->position[2] + this->Tmat[2];

    return res;
}

void
wlAnimatedMesh::glTransformations()
{
    glTranslated(this->Tmat[0], this->Tmat[1], this->Tmat[2]);
}

void
wlAnimatedMesh::makeList(int reset_camera)
{
    //  this->Print("Tmat: %lf %lf %lf", this->Tmat[0], this->Tmat[1], this->Tmat[2]);
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
