#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <stdexcept>

#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ParticleSimulator.h"
#include "DefaultParameters.h"

class ProjectView : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow * _ui;

    void _handleEvents();
    void _setDefaultParameters();

public:
    ProjectView();
    ~ProjectView();

    void setAxisIsDrawn();
    QGLViewer * getGLViewer();

    void setSize(int w, int h);

    void bindSimulator(ParticleSimulator & simu);

signals:
    void requestReset();

public slots:
    void reset();
    void update();
};

#endif // PROJECTVIEW_H
