#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <stdexcept>

#include <QMainWindow>

#include "ui_MainWindow.h"
#include "ParticleSimulator.h"
#include "DefaultParameters.h"

/// \brief The view for this project
class ProjectView : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow * _ui; /*!< The form object, from QtDesigner */

    /// \brief Connects event that are handle is the view
    void _handleEvents();
    /// \brief Defines the default values of the controls of the view
    void _setDefaultParameters();

public:
    ProjectView();
    ~ProjectView();

    /// \brief Tell if axis should be draw
    void setAxisIsDrawn();
    /// \brief Returns the inhner viewer
    QGLViewer * getGLViewer();

    /// \brief Sets the size of the view
    /// To be fixed, if enough time
    void setSize(int w, int h);

    /// \brief Binds a simulator to this view, so they can exchange signals;
    void bindSimulator(ParticleSimulator & simu);

signals:
    /// \brief Fires when reset action is triggered, to ask the bound simulator a reset action
    void requestReset();
    /// \brief Fires when GPU radio is checked, to ask the bound simulator to switch to GPU mode
    void requestGpuMode(bool gpuYesNo);

public slots:
    /// \brief Reset the view and emit request signal to the bounded simulator
    void reset();
    /// \brief Updates the QGLViewer
    void update();
};

#endif // PROJECTVIEW_H
