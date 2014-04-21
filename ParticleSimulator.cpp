#include "ParticleSimulator.h"

ParticleSimulator::ParticleSimulator(int debug, saViewer *viewer,
                                     wlSimulationEnvironment *environment,
                                     QVector<wlAnimatedMesh *> * items)
    :wlSimulator(debug, viewer, environment, items)
{
    this->Clear();
}

void ParticleSimulator::Clear()
{
    wlSimulator::Clear();

    _gpuMode = false;
    _coeff_d = 1.;
    _coeff_k = 3.;
    _coeff_mu = 0.1;
    _coeff_rho0 = 0.;

    _openClContext = NULL;
    _openClInput = NULL;
}

bool ParticleSimulator::isGPUMode()const
{
    return this->_gpuMode;
}

float ParticleSimulator::getPressureTolerance()const
{
    return this->_coeff_d;
}

float ParticleSimulator::getPressureToDensityGradientProportionnality()const
{
    return this->_coeff_k;
}

float ParticleSimulator::getDynamicViscosityConstant()const
{
    return this->_coeff_mu;
}

float ParticleSimulator::getInitialDensity() const
{
    return this->_coeff_rho0;
}

void ParticleSimulator::setSmoothingTolerance(const float & coeff_d) throw(std::invalid_argument)
{
    if(coeff_d <= 0)
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for distance tolerance");

    this->_coeff_d = coeff_d;
}

void ParticleSimulator::setPressureToDensityGradientProportionnality(const float & coeff_k)
{
    this->_coeff_k = coeff_k;
}

void ParticleSimulator::setDynamicViscosityConstant(const float & coeff_mu)
{
    this->_coeff_mu = coeff_mu;
}

void ParticleSimulator::setInitialDensity(const float & coeff_rho0) throw(std::invalid_argument)
{
    if(coeff_rho0 <= 0)
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for density");

    this->_coeff_rho0 = coeff_rho0;
}

void ParticleSimulator::setOpenClContext(QCLContext * openClContext,
                                         QCLVector<float> * openClInput)
{
    this->_gpuMode = true;

    this->_openClContext = openClContext;
    this->_openClInput = openClInput;

    this->_openClProgram = this->_openClContext->buildProgramFromSourceFile("./particules.c");
    this->_openClKernel = this->_openClProgram.createKernel("particules");
    this->_openClKernel.setGlobalWorkSize(this->items.size());
}

void ParticleSimulator::setGPUMode(const bool & gpuMode) throw(std::logic_error)
{
    if(gpuMode && (this->_openClContext==NULL || this->_openClInput==NULL))
    {
        throw std::logic_error("ParticleSimulator::setGPUMode: attempt to set GPU computation mode while OpenCl context not set.\n\t Consider using wlSimulator::setOpenClContext");
    }

    this->_gpuMode = gpuMode;
}

void ParticleSimulator::PrintSelf()
{
    wlSimulator::PrintSelf();

    this->Print("ParticleSimulator::PrintSelf");
    if(_gpuMode)
        this->Print("Mode de calcul : GPU");
    else
        this->Print("Mode de calcul : GPU");
    this->Print("Les constantes de la simulation : d=%f, k=%f, mhu=%f, rho_0=%f", _coeff_d, _coeff_k, _coeff_mu, _coeff_rho0);
}

void ParticleSimulator::_gpuStep()
{
    QCLVector<float> & openClInput = *this->_openClInput;

    this->_openClKernel(openClInput, (float)this->timestep);

    for(int i = 0; i < items.size(); ++i)
    {
        int index = i*6;
        items[i]->SetPosition(openClInput[index], openClInput[index+1], openClInput[index+2]);
        items[i]->SetVelocity(openClInput[index+3], openClInput[index+4], openClInput[index+5]);
    }
}

void ParticleSimulator::_cpuStep()
{
    // !! Move the 3 kernels to attributes later !!
    Poly6Kernel poly6(this->_coeff_d);
    SpikyKernel spiky(this->_coeff_d);
    ViscosityKernel viscy(this->_coeff_d);

    Particle * particle;

    //Compute densities first
    for(int i=0; i < items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(items[i]);

        if(particle)
        {
            particle->computeDensity(poly6, this->_coeff_rho0, this->_coeff_k);
        }

    }

    //compute velocities now
    for(int i=0; i < items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(items[i]);

        if(particle)
        {
            particle->computeTranslation(spiky, viscy, this->_coeff_mu);
        }
    }
}

void
ParticleSimulator::Step()
{
    this->Trace("-> Step()");

    if(this->_gpuMode)
    {
        _gpuStep();
    }

    else
    {
        _cpuStep();
    }

    // maintenant qu'on est est parvenu a calculer un pas de temps on met a jour l'affichage
    this->viewer->updateGL();
    this->Trace("<- Step()");
}
