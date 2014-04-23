#include "ProjectView.h"

ProjectView::ProjectView()
{
    _ui = new Ui::MainWindow();

    _ui->setupUi(this);

    _handleEvents();
    _setDefaultParameters();
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

void ProjectView::setSize(int w, int h)
{
    QSize size = this->size();

    float wRatio = w / size.width();
    float hRatio = h / size.height();

    _ui->viewer->setMinimumSize(w*wRatio, h*hRatio);
    this->setMinimumSize(w,h);
}

void ProjectView::_handleEvents()
{
    //Menu actions
    QObject::connect(_ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    QObject::connect(_ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));
}

void ProjectView::_setDefaultParameters()
{
    _ui->spinBox_timestep->setValue(DefaultParameters::TimeStep);
    _ui->spinBox_nbSteps->setValue(DefaultParameters::NbSteps);

    _ui->spinBox_d->setValue(DefaultParameters::Coeff_d);
    _ui->spinBox_mu->setValue(DefaultParameters::Coeff_mu);
    _ui->spinBox_k->setValue(DefaultParameters::Coeff_k);
    _ui->spinBox_rho0->setValue(DefaultParameters::Rho0);
    _ui->spinBox_mass->setValue(DefaultParameters::Mass);
}

void ProjectView::bindSimulator(ParticleSimulator & simu)
{
    ParticleSimulator * simulator = &simu;

    //Toolbar actions
    QObject::connect(this, SIGNAL(requestReset()), simulator, SLOT(reset())); // Reset
    QObject::connect(_ui->actionRestart, SIGNAL(triggered()), simulator, SLOT(restart())); // Restart
    QObject::connect(_ui->actionStep, SIGNAL(triggered()), simulator, SLOT(step())); // Step
    QObject::connect(_ui->actionPlay, SIGNAL(triggered()), simulator, SLOT(play())); // Play
    QObject::connect(_ui->actionStop, SIGNAL(triggered()), simulator, SLOT(stop())); // Stop

    //Simulation Parameters controls
    QObject::connect(_ui->spinBox_mu, SIGNAL(valueChanged(double)), simulator, SLOT(setDynamicViscosityConstant(const double &))); // Coeff mu
    QObject::connect(_ui->spinBox_d, SIGNAL(valueChanged(double)), simulator, SLOT(setSmoothingTolerance(const double &))); // Coeff d
    QObject::connect(_ui->spinBox_k, SIGNAL(valueChanged(double)), simulator, SLOT(setPressureToDensityGradientProportionnality(const double &))); // Coeff k
    QObject::connect(_ui->spinBox_rho0, SIGNAL(valueChanged(double)), simulator, SLOT(setInitialDensity(const double &))); // Ref density
    QObject::connect(_ui->spinBox_mass, SIGNAL(valueChanged(double)), simulator, SLOT(setParticlesMass(const double &))); // Particles mass
    QObject::connect(_ui->spinBox_timestep, SIGNAL(valueChanged(double)), simulator, SLOT(setTimeStep(const double &))); // Time step
    QObject::connect(_ui->spinBox_nbSteps, SIGNAL(valueChanged(int)), simulator, SLOT(setNumberOfTimeSteps(int))); // Number of steps
}

void ProjectView::reset()
{
    _setDefaultParameters();

    emit requestReset();
}
