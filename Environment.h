#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <qglviewer.h>

#include <stdexcept>
#include <QVector>
#include <QVector3D>
#include "DefaultParameters.h"

/// \brief Represents the boundaries of the simulation
class Environment
{
public:
    Environment();

    /// \brief Provides the environment limits on each axis.
    /// The environment is represented by a simple cube-like geometry
    /// return vector has the form <xMin, xMax, yMin, yMax, zMin, zMax>
    QVector<float> getLimits()const;
    /// \brief Defines the limits of the environment
    /// The environment is represented by a simple cube-like geometry
    /// the vector argument must have the form <xMin, xMax, yMin, yMax, zMin, zMax>
    void setLimits(const QVector<float> & limits) throw(std::invalid_argument);
    /// \brief Provides the minimum values in a QVector3D
    QVector3D getMinVector3D()const;
    /// \brief Provides the minimum values in a QVector3D
    QVector3D getMaxVector3D()const;
    /// \brief Provides the minimum value for x
    float getXMin()const;
    /// \brief Provides the maximum value for x
    float getXMax()const;
    /// \brief Provides the minimum value for y
    float getYMin()const;
    /// \brief Provides the maximum value for y
    float getYMax()const;
    /// \brief Provides the minimum value for z
    float getZMin()const;
    /// \brief Provides the maximum value for z
    float getZMax()const;
    /// \brief Draws the parallelepipede that corresponds to the limits
    void draw()const;

private:
    QVector<float> _limits; /*!< Contains 6 values, that represent min and max on each axis */
};

#endif // ENVIRONMENT_H
