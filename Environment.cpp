#include "Environment.h"

Environment::Environment()
{
    _limits << DefaultParameters::EnvironmentLimits[0]
            << DefaultParameters::EnvironmentLimits[1]
            << DefaultParameters::EnvironmentLimits[2]
            << DefaultParameters::EnvironmentLimits[3]
            << DefaultParameters::EnvironmentLimits[4]
            << DefaultParameters::EnvironmentLimits[5];
}

QVector<float> Environment::getLimits() const
{
    return _limits;
}

void Environment::setLimits(const QVector<float> &limits) throw(std::invalid_argument)
{
    if(limits.size() != 6)
        throw std::invalid_argument("Environment::setLimits : argument must be a 6 length vector");

    _limits = limits;
}

float Environment::getXMin() const
{
    return _limits[0];
}

float Environment::getXMax() const
{
    return _limits[1];
}

float Environment::getYMin() const
{
    return _limits[2];
}

float Environment::getYMax() const
{
    return _limits[3];
}

float Environment::getZMin() const
{
    return _limits[4];
}

float Environment::getZMax() const
{
    return _limits[5];
}

void Environment::draw() const
{
    const float & xMin = _limits[0];
    const float & xMax= _limits[1];
    const float & yMin = _limits[2];
    const float & yMax = _limits[3];
    const float & zMin = _limits[4];
    const float & zMax= _limits[5];

    glPushMatrix();
    glColor4f(1., 1., 1., 0.1);
    glBegin(GL_TRIANGLES);
        /**** BOTTOM *****/
        //1st triangle
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMin, yMin, zMin);
        //2nd triangle
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);

        /**** FRONT *****/
        //1st triangle
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMax, yMin, zMin);
        //2nd triangle
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMin, zMin);

        /**** BACK *****/
        //1st triangle
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMax);
        //2nd triangle
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMin, yMax, zMax);

        /**** LEFT *****/
        //1st triangle
        glVertex3f(xMin, yMax, zMax);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMin, yMin, zMax);
        //2nd triangle
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMin, yMin, zMin);

        /**** RIGHT *****/
        //1st triangle
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMin);
        //2nd triangle
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);
    glEnd();
    glPopMatrix();
}
