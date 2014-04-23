#include "ProjectView.h"

ProjectView::ProjectView()
{
    _ui = new Ui::MainWindow();

    _ui->setupUi(this);

    _handleEvents();
}

ProjectView::~ProjectView()
{
    delete(_ui);
}

void ProjectView::setAxisIsDrawn()
{
    _ui->viewer->setAxisIsDrawn();
}

saViewer *ProjectView::getGLViewer()
{
    return _ui->viewer;
}

void ProjectView::_handleEvents()
{
    //Menu actions
    QObject::connect(_ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}

void ProjectView::bindSimulator(ParticleSimulator & simu)
{
    ParticleSimulator * simulator = &simu;

    //Toolbar actions
    QObject::connect(_ui->actionReset, SIGNAL(triggered()), simulator, SLOT(Reset())); // reset
    QObject::connect(_ui->actionStep, SIGNAL(triggered()), simulator, SLOT(Step())); // Step
    QObject::connect(_ui->actionPlay, SIGNAL(triggered()), simulator, SLOT(Play())); //Play
    QObject::connect(_ui->actionStop, SIGNAL(triggered()), simulator, SLOT(Stop())); // Stop

    //Simulation Parameters controls
    QObject::connect(_ui->spinBox_mu, SIGNAL(valueChanged(double)), simulator, SLOT(setDynamicViscosityConstant(const double &))); // Coeff mu
    QObject::connect(_ui->spinBox_d, SIGNAL(valueChanged(double)), simulator, SLOT(setSmoothingTolerance(const double &))); // Coeff d
    QObject::connect(_ui->spinBox_k, SIGNAL(valueChanged(double)), simulator, SLOT(setPressureToDensityGradientProportionnality(const double &))); // Coeff k
    QObject::connect(_ui->spinBox_rho0, SIGNAL(valueChanged(double)), simulator, SLOT(setInitialDensity(const double &))); // Ref density
    QObject::connect(_ui->spinBox_mass, SIGNAL(valueChanged(double)), simulator, SLOT(setParticlesMass(const double &))); // Particles mass
    QObject::connect(_ui->spinBox_timestep, SIGNAL(valueChanged(double)), simulator, SLOT(SetTimeStep(const double &))); // Time step
    QObject::connect(_ui->spinBox_nbSteps, SIGNAL(valueChanged(int)), simulator, SLOT(SetNumberOfTimeSteps(int))); // Number of steps
}
