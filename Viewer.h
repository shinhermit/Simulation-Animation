#ifndef VIEWER_H
#define VIEWER_H

#include <qglviewer.h>

#include "Simulator.h"

/// A ligthly extended GLViewer for the simulation. Extends QGLViewer, sothat it is bound
/// to a Simulator
class Viewer : public QGLViewer
{
    Q_OBJECT

public:
    Viewer(QWidget * parent = 0, const QGLWidget * shareWidget = 0,
           Qt::WindowFlags flags = 0);

    Viewer(QGLContext * context, QWidget * parent = 0, const QGLWidget * shareWidget = 0,
            Qt::WindowFlags flags = 0 );

    Viewer(const QGLFormat & format, QWidget * parent = 0, const QGLWidget * shareWidget = 0,
            Qt::WindowFlags flags = 0 );

    /// \brief Binds a simulator to this view
    void setSimulator(Simulator & simulator);

protected:
    void draw();

private:
    Simulator * _simulator; /*!< A simulator bounded to this view */
};

#endif // VIEWER_H
