#include "Viewer.h"

Viewer::Viewer(QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags)
    :QGLViewer(parent, shareWidget, flags)
{
    connect(this, SIGNAL(drawNeeded()), this, SLOT(draw()));
}

Viewer::Viewer(QGLContext *context, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags)
    :QGLViewer(context, parent, shareWidget, flags)
{
    connect(this, SIGNAL(drawNeeded()), this, SLOT(draw()));
}

Viewer::Viewer(const QGLFormat & format, QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags)
    :QGLViewer(format, parent, shareWidget, flags)
{
    connect(this, SIGNAL(drawNeeded()), this, SLOT(draw()));
}

void Viewer::setSimulator(Simulator &simulator)
{
    _simulator = &simulator;
}

void Viewer::draw()
{
    //_simulator->draw();
}
