#include "ParticleSimulator.h"

ParticleSimulator::ParticleSimulator(const unsigned int & nbParticles, int debug, saViewer *viewer,
                                     wlSimulationEnvironment *environment)
    :wlSimulator(debug, viewer, environment, new QVector<wlAnimatedMesh*>())
{
    this->Clear();
    _createParticles(nbParticles, debug);
    std::cerr << "ParticleSimulator::ParticleSimulator: nbParticles: " << items.size() << std::endl;
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

void ParticleSimulator::_createParticles(const unsigned int & nbItems, const int & debug)
{
    Particle * particle;
    float Xp, Yp, Zp, zOffset;
    unsigned int index;

    double itemsPerSide = ::pow(nbItems, 1./3);
    float step = 3;
    zOffset = 50;

    for(unsigned int i=0; i < itemsPerSide; ++i)
    {
        Xp = i*step;
        for(unsigned int j=0; j < itemsPerSide; ++j)
        {
            Yp = j*step;
            for(unsigned int k=0; k < itemsPerSide; ++k)
            {
                Zp = k*step + zOffset;

                particle = new Particle(this->items, debug, NULL, "sphere.off");
                particle->SetPosition(Xp, Yp, Zp);
                particle->SetVelocity(0, 0, 0);

                // Problem: à ce niveau, _openClInput n'est pas encore défini
                // (setOpenClContext pas encore appelée)
                if(_gpuMode)
                {
                    index = i + j*itemsPerSide + k*itemsPerSide*itemsPerSide;
                    QCLVector<float> & clInput = *_openClInput;
                    clInput[index] = Xp;
                    clInput[index+1] = Yp;
                    clInput[index+2] = Zp;

                    clInput[index+3] = 0;
                    clInput[index+4] = 0;
                    clInput[index+5] = 0;
                }

                particle->SetReaction(PURE_KINEMATIC);
                particle->SetAttenuationCoefficientForPureKinematicReaction(0.8);
                particle->Reset();

                this->AddItem(particle);
            }
        }
    }

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

void ParticleSimulator::setSmoothingTolerance(const double & coeff_d) throw(std::invalid_argument)
{
    if(coeff_d <= 0)
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for distance tolerance");

    this->_coeff_d = coeff_d;
}

void ParticleSimulator::setPressureToDensityGradientProportionnality(const double & coeff_k)
{
    this->_coeff_k = coeff_k;
}

void ParticleSimulator::setDynamicViscosityConstant(const double & coeff_mu)
{
    this->_coeff_mu = coeff_mu;
}

void ParticleSimulator::setInitialDensity(const double & coeff_rho0) throw(std::invalid_argument)
{
    if(coeff_rho0 <= 0)
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for density");

    this->_coeff_rho0 = coeff_rho0;
}

void ParticleSimulator::setParticlesMass(const double & mass) throw(std::invalid_argument)
{
    for(int i=0; i < items.size(); ++i)
    {
        Particle * particle = dynamic_cast<Particle*>(items[i]);

        particle->setMass(mass);
    }
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
