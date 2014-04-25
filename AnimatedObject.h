#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <QObject>
#include <QVector>
#include <cstdio>

#include "wlCore.h"
#include "DefaultParameters.h"

class AnimatedObject : public QObject, public wlCore
{
    Q_OBJECT

public:
    AnimatedObject(int debug=0);

    virtual ~AnimatedObject();
    virtual char *getClassName()const;

  public slots:
    /// \brief Defines the duration of a simulation's step
    virtual void setTimeStep(const float & timestep);

    /// \brief Defines the initial position of the object.
    virtual void setInitialPosition(const float & x, const float & y, const float & z);
    /// \brief Defines the current position of the object. Actually updates the translation vector.
    virtual void setPosition(const float & x, const float & y, const float & z);

    /// \brief Defines the initial velocity of the object.
    virtual void setInitialVelocity(const float & vX, const float & vY, const float & vZ);
    /// \brief Defines the velocity of the object.
    virtual void setVelocity(const float & vX, const float & vY, const float & vZ);
    /// \brief Increments the local simulation step counter. Useful for update after GPU computation.
    virtual void newStep();

    /// \brief Return the velocity of the object.
    virtual QVector<float> getVelocity()const;
    /// \brief Return the position of the object.
    virtual QVector<float> getPosition()const;
    /// \brief Returns the duration of the simulation step.
    virtual float getTimeStep()const;
    /// \brief Return the current step number.
    virtual float getCurrentStep()const;

    /// \brief Print the current state of the object.
    virtual void printSelf()const;

    /// \brief Resets the object to its initial configuration.
    virtual void reset();
    /// \brief Executes one simulation step.
    virtual void step();

  protected:
    // Paramètres initiaux
    float _timestep; /*!< Duration of a simulation step */

    QVector<float> _initPos; /*!< Initial position */
    QVector<float> _initVel; /*!< Initial velocity */
    QVector<float> _initAcc; /*!< Initial acceleration */

    // Propriétés du pas de temps
    int _step; /*!< Current step */
    QVector<float> _vel; /*!< Current velocity */
    QVector<float> _acc; /*!< Current acceleration */
    QVector<float> _tVec; /*!< Current translation vector */

    /// \brief Reinitializes all the properties
    virtual void _clear();
};

#endif // ANIMATEDOBJECT_H
