#ifndef VIEWER_H
#define VIEWER_H

#include <qglviewer.h>

#include "Simulator.h"

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

    void setSimulator(Simulator & simulator);

protected:
    void draw();

private:
    Simulator * _simulator;
};

#endif // VIEWER_H
