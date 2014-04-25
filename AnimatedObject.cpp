#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(int debug)
    :wlCore(debug)
{
    this->Trace("-> AnimatedObject()");

    _initVel << 0.0 << 0.0 << 0.0;
    _initPos << 0.0 << 0.0 << 0.0;
    _tVec << 0.0 << 0.0 << 0.0;

    _clear();

    this->Trace("<- AnimatedObject()");
}

void AnimatedObject::_clear()
{
    _timestep = DefaultParameters::TimeStep;
    _step = 0;

    _vel = _initVel;

    _tVec.fill(0.);

    this->Modified("Position");
}

AnimatedObject::~AnimatedObject()
{}

char *AnimatedObject::getClassName() const
{
    return "AnimatedObject";
}

void AnimatedObject::setTimeStep(const float &  timestep)
{
    _timestep = timestep;
}

QVector<float> AnimatedObject::getPosition() const
{
    QVector<float> p;

    p << _initPos[0] + _tVec[0]
      << _initPos[1] + _tVec[1]
      << _initPos[2] + _tVec[2];
    return p;
}

float AnimatedObject::getTimeStep() const
{
    return _timestep;
}

float AnimatedObject::getCurrentStep() const
{
    return _step;
}

QVector<float> AnimatedObject::getVelocity() const
{
    return _vel;
}


void AnimatedObject::printSelf() const
{
    QVector<float> pos = getPosition();

    this->Print("Initial Position : (%.2f, %.2f, %.2f)", _initPos[0], _initPos[1], _initPos[2]);
    this->Print("INitial Speed : (%.2f, %.2f, %.2f)", _initVel[0], _initVel[1], _initVel[2]);
    this->Print("Current Speed : (%.2f, %.2f, %.2f)", _vel[0], _vel[1], _vel[2]);
    this->Print("Current Potision : (%.2f, %.2f, %.2f)", pos[0], pos[1], pos[2]);
}

void AnimatedObject::setInitialPosition(const float & x, const float & y, const float & z)
{
    _initPos[0] = x;
    _initPos[1] = y;
    _initPos[2] = z;
}

void AnimatedObject::setPosition(const float & x, const float & y, const float & z)
{
    QVector<float> p = getPosition();

    _tVec[0] += x - p[0];
    _tVec[1] += y - p[1];
    _tVec[2] += z - p[2];

    this->Modified("Position");
    this->Modified("DisplayList");
}

void AnimatedObject::setInitialVelocity(const float & vX, const float & vY, const float & vZ)
{
    _initVel[0] = vX;
    _initVel[1] = vY;
    _initVel[2] = vZ;
}

void AnimatedObject::setVelocity(const float & vX, const float & vY, const float & vZ)
{
    _vel[0] = vX;
    _vel[1] = vY;
    _vel[2] = vZ;
}

void AnimatedObject::newStep()
{
    ++ _step;
}

void AnimatedObject::reset()
{
    this->Trace("-> reset()");

    _tVec.fill(0);
    _vel = _initVel;
    _step = 0;

    this->Modified("Position");
    this->Modified("DisplayList");

    this->Trace("<- reset()");
}

void AnimatedObject::step()
{
    this->Trace("-> step()");

    float G = DefaultParameters::Gravity;

    _tVec[0] += _vel[0] * _timestep;
    _tVec[1] += _vel[1] * _timestep;
    _tVec[2] += (_vel[2] - G * _timestep) / 2 * _timestep;

    _vel[2] -= G * _timestep;

    ++_step;

    this->Modified("Position");
    this->Modified("DisplayList");
    this->Trace("<- step()");
}
