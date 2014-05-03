#include "Particle.h"

Particle::Particle(const QVector<AnimatedObject *> & everyone, const Environment & env)
    : _env(env),
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

    for(unsigned int i=0; i < (unsigned int)_everyone.size(); ++i)
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

//    this->_density = (density <= 0.) ? .000001 : density; // Pour les tests uniquement, résoudre plus tard
    this->_density = density;
    this->_pressure = coeff_k*(density - refDensity);
}

// !!! To be checked: divisions by density (density != 0 ?)
// If other's particle density is null, then we are not in this particle influence distance,
// because in that, the density of this other particle would have at least included our influence
// and thus could not have been null.
void Particle::computeTranslation(const SPHKernel & kernelP, const SPHKernel & kernelV, const float & coeff_mu)
{
    Particle * other;
    QVector<float> R_ij;
    QVector<float> gradP, laplV;
    QVector<float> gradK;
    QVector<float> acc;
    QVector<float> velOther;
    QVector<float> myPos, hisPos;
    float coeff;
    float xCstrt, yCstrt, zCstrt;

    gradP << 0 << 0 << 0;
    laplV << 0 << 0 << 0;

    myPos = this->getPosition();

    for(unsigned int i=0; i < (unsigned int)_everyone.size(); ++i)
    {
        if(_everyone[i] != this)
        {
            other = dynamic_cast<Particle*>(_everyone[i]);
            if(other)
            {
                float otherDensity = other->getDensity();

                hisPos = other->getPosition();
                R_ij << myPos[0] - hisPos[0]
                     << myPos[1] - hisPos[1]
                     << myPos[2] - hisPos[2];

                // Gradient de la pression
                coeff = 0;
                if(otherDensity != 0)
                    coeff = 0.5 * other->getMass() * (this->_pressure + other->getPressure()) / otherDensity;
                gradK = kernelP.gradient(R_ij);

                gradP[0] += coeff*gradK[0];
                gradP[1] += coeff*gradK[1];
                gradP[2] += coeff*gradK[2];

                //Laplacien de la vitesse
                coeff = 0;
                if(otherDensity)
                    coeff = other->getMass() * kernelV.laplacian(R_ij) / otherDensity;
                velOther = other->getVelocity();

                laplV[0] += coeff*(velOther[0] - _vel[0]);
                laplV[1] += coeff*(velOther[1] - _vel[1]);
                laplV[2] += coeff*(velOther[2] - _vel[2]);
            }
        }
    }

    //calcul de l'accélération et de la vitesse
    // The gravity
    acc << 0 << 0 << -9.8;

    // Environment constraints (limits)
    xCstrt = (myPos[0] <= _env.getXMin() || myPos[0]>= _env.getXMax()) ? -1 : 1;
    yCstrt = (myPos[1] <= _env.getYMin() || myPos[1]>= _env.getYMax()) ? -1 : 1;
    zCstrt = (myPos[2] <= _env.getZMin()) ? -1 : 1;

    // The influences
    if(_density)
    {
        acc[0] += xCstrt *( (coeff_mu*laplV[0] - gradP[0])/_density );
        acc[1] += yCstrt *( (coeff_mu*laplV[1] - gradP[1])/_density );
        acc[2] += zCstrt *( (coeff_mu*laplV[2] - gradP[2])/_density );
    }

    ++ _step;

    //La vitesse
    QVector<float> v0 = _vel; //Save v_0

    _vel[0] = xCstrt*( v0[0] + acc[0]*_timestep );
    _vel[1] = yCstrt*( v0[1] + acc[1]*_timestep );
    _vel[2] = zCstrt*( v0[2] + acc[2]*_timestep );

    //La translation
    // x = 1/2*a*t^2 + v_0*t + x_0
    // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
    float time = _timestep * _step;
    float time_p = time - _timestep;
    _tVec[0] += xCstrt*( 0.5*acc[0]*(time*time - time_p*time_p) + v0[0]*_timestep );
    _tVec[1] += yCstrt*( 0.5*acc[1]*(time*time - time_p*time_p) + v0[1]*_timestep );
    _tVec[2] += zCstrt*( 0.5*acc[2]*(time*time - time_p*time_p) + v0[2]*_timestep );
}

void Particle::printSelf() const
{
    AnimatedObject::printSelf();

    std::cout << "Density : " << _density << std::endl;
    std::cout << "Pressure: " << _pressure << std::endl;
}

void Particle::step()
{}

void Particle::setDensity(const float &density)
{
    _density = density;
}

void Particle::setPressure(const float &pressure)
{
    _pressure = pressure;
}

void Particle::reset()
{
    AnimatedObject::reset();

    _mass = DefaultParameters::Mass;
    _density = DefaultParameters::Density;
    _pressure = DefaultParameters::Pressure;
}
