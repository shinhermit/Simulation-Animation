#include "Particle.h"

Particle::Particle(const QVector<AnimatedObject *> & everyone, int debug)
    : AnimatedObject(debug),
      _everyone(everyone)
{
    _clear();
}

void Particle::_clear()
{
    AnimatedObject::_clear();

    _mass = DefaultParameters::Mass;
    _density = DefaultParameters::Density;
    _pressure = DefaultParameters::Pressure;
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
    QVector<float> myPos, hisPos;

    myPos = this->getPosition();

    for(int i=0; i < _everyone.size(); ++i)
    {
        if(_everyone[i] != this)
        {
            other = dynamic_cast<Particle*>(_everyone[i]);
            if(other)
            {
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

// !!! A vérifier: les division par density (density != 0 ?)
void Particle::computeTranslation(const SPHKernel & kernelP, const SPHKernel & kernelV, const float & coeff_mu)
{
    Particle * other;
    QVector<float> R_ij;
    QVector<float> gradP, laplV;
    QVector<float> gradK;
    QVector<float> acc, gravity;
    QVector<float> velOther;
    QVector<float> myPos, hisPos;
    float coeff;

    gradP << 0 << 0 << 0;
    laplV << 0 << 0 << 0;

    myPos = this->getPosition();

    for(int i=0; i < _everyone.size(); ++i)
    {
        if(_everyone[i] != this)
        {
            other = dynamic_cast<Particle*>(_everyone[i]);
            if(other)
            {
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
                velOther = other->getVelocity();

                laplV[0] += coeff*(velOther[0] - _cvel[0]);
                laplV[1] += coeff*(velOther[1] - _cvel[1]);
                laplV[2] += coeff*(velOther[2] - _cvel[2]);
            }
        }
    }

//    _displayBefore();

    //calcul de l'accélération et de la vitesse
    if(this->getPosition()[2] > 0.1)
    {
        gravity << 0 << 0 << -9.8;
    }
    else
    {
        gravity << 0 << 0 << 0;
        _cvel.fill(0);
    }

    acc << gravity[0] + (coeff_mu*laplV[0] - gradP[0])/this->getDensity()
        << gravity[1] + (coeff_mu*laplV[1] - gradP[1])/this->getDensity()
        << gravity[2] + (coeff_mu*laplV[2] - gradP[2])/this->getDensity();

    ++ _cstep;

    //La vitesse
    QVector<float> v0 = _cvel; //Save v_0

    _cvel[0] += acc[0]*_timestep;
    _cvel[1] += acc[1]*_timestep;
    _cvel[2] += acc[2]*_timestep;

    //La translation
    // x = 1/2*a*t^2 + v_0*t + x_0
    // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
    float time = _timestep * _cstep;
    float time_p = time - _timestep;
    _tVec[0] += 0.5*acc[0]*(time*time - time_p*time_p) + v0[0]*_timestep;
    _tVec[1] += 0.5*acc[1]*(time*time - time_p*time_p) + v0[1]*_timestep;
    _tVec[2] += 0.5*acc[2]*(time*time - time_p*time_p) + v0[2]*_timestep;

//    _displayAfter(acc);

    this->Modified("Position");
    this->Modified("DisplayList");
}

void Particle::_displayBefore() const
{
    std::cerr << "Particle::computeVelocity: velocity before computation is ("
              << _cvel[0] << ", " << _cvel[1] << ", " << _cvel[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: position before computation is ("
              << this->getPosition()[0] << ", " << this->getPosition()[1] << ", "
              << this->getPosition()[2] << ")" << std::endl;
    std::cerr << std::endl;
}

void Particle::_displayAfter(const QVector<float> & acc) const
{
    QVector<float> p = this->getPosition();

    std::cerr << "Particle::computeVelocity: this->timestep is " << _timestep << std::endl;
    std::cerr << "Particle::computeVelocity: computed acceleration is ("
              << acc[0] << ", " << acc[1] << ", " << acc[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: velocity after computation is ("
              << _cvel[0] << ", " << _cvel[1] << ", " << _cvel[2] << ")" << std::endl;
    std::cerr << "Particle::computeVelocity: position after computation is ("
              << p[0] << ", " << p[1] << ", " << p[2] << ")" << std::endl;
    std::cerr << std::endl << std::endl;
}

void Particle::step()
{}

void Particle::reset()
{
    AnimatedObject::reset();

    _mass = DefaultParameters::Mass;
    _density = DefaultParameters::Density;
    _pressure = DefaultParameters::Pressure;
}
