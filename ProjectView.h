#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <stdexcept>

#include <QMainWindow>

#include "wlSimulator.h"
#include "ui_MainWindow.h"
#include "ParticleSimulator.h"

class ProjectView : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow * _ui;

    void _handleEvents();

public:
    ProjectView();
    ~ProjectView();

    void setAxisIsDrawn();
    saViewer * getGLViewer();

    void bindSimulator(ParticleSimulator & simu);
};

#endif // PROJECTVIEW_H
