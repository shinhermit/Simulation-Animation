#include "Particle.h"

Particle::Particle(const QVector<wlAnimatedMesh *> & others,
                   int debug, wlQGLViewer *const v, QString filename)
    : wlAnimatedSphere(debug, v, filename),
      _others(others)
{
    Clear();
}

void Particle::Clear()
{
    wlAnimatedSphere::Clear();

    _mass = 1.0;
    _density = 0.;
    _pressure = 0.;

    _previous_density = 0.;
    _previous_pressure = 0.;
}

void Particle::setMass(const float & mass) throw(std::invalid_argument)
{
    if(mass < 0)
        throw std::invalid_argument("wlAnimatedMesh::SetMass: mass can not be negative.");

    this->_mass = mass;
}

float Particle::getMass()const
{
    return _mass;
}

float Particle::getDensity()const
{
    return _density;
}

float Particle::getPressure()const
{
    return _pressure;
}

void Particle::computeDensity(const SPHKernel & kernel, const float & refDensity, const float & coeff_k)
{
    float density = 0;
    Particle * other;
    QVector<float> R_ij;
    QVector<double> myPos, hisPos;

    for(int i=0; i < _others.size(); ++i)
    {
        if(_others[i] != this)
        {
            other = dynamic_cast<Particle*>(_others[i]);
            if(other)
            {
                myPos = this->getPosition();
                hisPos = other->getPosition();
                R_ij << myPos[0] - hisPos[0]
                     << myPos[1] - hisPos[1]
                     << myPos[2] - hisPos[2];

                density += other->getMass() * kernel(R_ij);
            }
        }
    }

    this->_density = (density <= 0.) ? 1. : density; // Pour les tests uniquement, résoudre plus tard
    this->_pressure = coeff_k*(density - refDensity);
}


void Particle::_displayBefore() const
{
    std::cerr << "Particle::computeVelocity: velocity before computation is ("
              << this->cvel[0] << ", " << this->cvel[1] << ", " << this->cvel[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: position before computation is ("
              << this->getPosition()[0] << ", " << this->getPosition()[1] << ", "
              << this->getPosition()[2] << ")" << std::endl;
    std::cerr << std::endl;
}

void Particle::_displayAfter(const QVector<float> & acc) const
{
    std::cerr << "Particle::computeVelocity: this->timestep is " << this->timestep << std::endl;
    std::cerr << "Particle::computeVelocity: computed acceleration is ("
              << acc[0] << ", " << acc[1] << ", " << acc[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: velocity after computation is ("
              << this->cvel[0] << ", " << this->cvel[1] << ", " << this->cvel[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: position after computation is ("
              << this->getPosition()[0] << ", " << this->getPosition()[1] << ", "
              << this->getPosition()[2] << ")" << std::endl;
    std::cerr << std::endl << std::endl;
}

// !!! A vérifier: les division par density (density != 0 ?)
void Particle::computeTranslation(const SPHKernel & kernelP, const SPHKernel & kernelV, const float & coeff_mu)
{
    Particle * other;
    QVector<float> R_ij;
    QVector<float> gradP, laplV;
    QVector<float> gradK;
    QVector<float> acc, gravity;
    QVector<double> velOther;
    QVector<double> myPos, hisPos;
    float coeff;

    gradP << 0 << 0 << 0;
    laplV << 0 << 0 << 0;

    for(int i=0; i < _others.size(); ++i)
    {
        if(_others[i] != this)
        {
            other = dynamic_cast<Particle*>(_others[i]);
            if(other)
            {
                myPos = this->getPosition();
                hisPos = other->getPosition();
                R_ij << myPos[0] - hisPos[0]
                     << myPos[1] - hisPos[1]
                     << myPos[2] - hisPos[2];

                // Gradient de la pression
                coeff = 0.5 * other->getMass() * (this->_pressure + other->getPressure()) / other->getDensity();
                gradK = kernelP.gradient(R_ij);

                gradP[0] += coeff*gradK[0];
                gradP[1] += coeff*gradK[1];
                gradP[2] += coeff*gradK[2];

                //Laplacien de la vitesse
                coeff = other->getMass() * kernelV.laplacian(R_ij) / other->getDensity();
                velOther = other->GetVelocity();

                laplV[0] += coeff*(velOther[0] - this->cvel[0]);
                laplV[1] += coeff*(velOther[1] - this->cvel[1]);
                laplV[2] += coeff*(velOther[2] - this->cvel[2]);
            }
        }
    }

//    _displayBefore();

    //calcul de l'accélération et de la vitesse
    gravity << 0 << 0 << -9.8;
    acc << gravity[0] + (-gradP[0] + coeff_mu*laplV[0])/this->getDensity()
        << gravity[1] + (-gradP[1] + coeff_mu*laplV[1])/this->getDensity()
        << gravity[2] + (-gradP[2] + coeff_mu*laplV[2])/this->getDensity();

    ++ this->cstep;

    //La vitesse
    this->cvel[0] += acc[0]*this->timestep;
    this->cvel[1] += acc[1]*this->timestep;
    this->cvel[2] += acc[2]*this->timestep;

    //La translation
    this->Tmat[0] += 0.5*acc[0]*this->timestep*this->timestep*this->cstep;
    this->Tmat[1] += 0.5*acc[1]*this->timestep*this->timestep*this->cstep;
    this->Tmat[2] += 0.5*acc[2]*this->timestep*this->timestep*this->cstep;

//    _displayAfter(acc);

    this->Modified("Position");
    this->Modified("DisplayList");
}

void Particle::Step()
{}

void Particle::Back()
{
    wlAnimatedSphere::Back();

    _density = _previous_density;
    _pressure = _previous_pressure;
}

void Particle::Reset()
{
    wlAnimatedSphere::Reset();

    _density = 0.;
    _pressure = 0.;

    _previous_density = 0.;
    _previous_pressure = 0.;
}
