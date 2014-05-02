#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <qglviewer.h>

#include "wlCore.h"
#include "wlSimulationEnvironment.h"
#include "AnimatedObject.h"
#include "Environment.h"

class Simulator : public QObject, public wlCore
{
    Q_OBJECT

public:
    /// Creates a new kinematic simulator
    /// <em>viewer</em> OpenGL viewer that is to be associated with the simulator.
    /// <em>environment</em> Motion-less object in the simulation.
    /// <em>items</em> Animated objects.
    Simulator(int debug=0, QGLViewer *viewer=NULL, QVector<AnimatedObject*> * items=NULL);
    /// Destructor.
    virtual ~Simulator();
    /// \brief Needed for wlCore inheritance.
    virtual char *getClassName()const;

    /// \brief Adds a new animated object.
    virtual void addItem(AnimatedObject * item);
    /// \brief Remove all the animated objects.
    virtual void clearItems();

signals:
    /// \brief Signal to request an update of opengl display.
    void requestUpdateGL();

public slots:
    /// \brief Defines the number steps of the simulation.
    void setNumberOfTimeSteps(const int & nbSteps);
    /// \brief Defines the duration of a simulation step.
    void setTimeStep(const double & timestep);

    /// \brief Resets the simulation.
    /// All the simulation parameters will be set to default.
    virtual void reset();
    /// \brief Moves back all animated objects to their inital positions.
    virtual void restart();
    /// \brief Computes one step of the simulation.
    virtual void step();
    /// \brief Computes several steps of the simulation.
    /// start a timer that will periodically compute one step, until the defined simulation's number of step is reeached.
    virtual void play();
    /// \brief Stops a previously played simulation.
    virtual void stop();

    /// \brief Draws all objects.
    virtual void draw();

    /// Displays the simulation properties.
    virtual void printSelf();

protected:
    Environment _env; /*!< Cubic limits of the scene */
    QVector<AnimatedObject*> & _items; /*!< All animated objects */
    QGLViewer *_viewer; /*!< The opengl viewer */
    float _timestep; /*!< The duration of one step */
    unsigned int _nsteps; /*!< The number os steps of the simulation */
    unsigned int _cstep; /*!< The steps counter */
    QTimer * _timer; /*!< Timer, for simulation playing */

    /// \brief Reset all properties to default
    virtual void _clear();

    /// \brief Sets the scene up.
    /// Ensures that all objects are visible.
    virtual void _setupScene();
};

#endif // SIMULATOR_H
