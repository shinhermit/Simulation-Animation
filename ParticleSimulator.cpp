#include "ParticleSimulator.h"

ParticleSimulator::ParticleSimulator(QGLViewer *viewer)
    :Simulator(viewer, new QVector<AnimatedObject*>())
{
    _clear();
    ::srand(::time(NULL));
}

void ParticleSimulator::_clear()
{
    Simulator::_clear();

    _gpuMode = DefaultParameters::GpuMode;
    _coeff_d = DefaultParameters::Coeff_d;
    _coeff_k = DefaultParameters::Coeff_k;
    _coeff_mu = DefaultParameters::Coeff_mu;
    _coeff_rho0 = DefaultParameters::Rho0;

    _openClContext = NULL;
    _openClInput = NULL;
    _openClOutput = NULL;
}

void ParticleSimulator::createParticles(const unsigned int & nbItems)
{
    Particle * particle;
    float Xp, Yp, Zp, Xv, Yv, Zv, zOffset;
    float xMin, xMax, yMin, yMax, zMin, zMax;
    float vMin, vMax;
    float padding;
    unsigned int index;

    padding = 0.05;
    zOffset = 4./5;
    xMin = _env.getXMin() - padding*_env.getXMin();
    xMax = _env.getXMax() - padding*_env.getXMax();
    yMin = _env.getYMin() - padding*_env.getYMin();
    yMax = _env.getYMax() - padding*_env.getYMax();
    zMin = _env.getZMin() + zOffset*(_env.getZMax()-_env.getZMin());
    zMax = _env.getZMax();

    vMin = -3.;
    vMax = 3.;

    QCLVector<float> & clInput = *_openClInput;

    for(unsigned int i=0; i < nbItems; ++i)
    {
        Xp = xMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(xMax-xMin));
        Yp = yMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(yMax-yMin));
        Zp = zMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(zMax-zMin));

        Xv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));
        Yv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));
        Zv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));

        particle = new Particle(_items, _env);
        particle->setInitialPosition(Xp, Yp, Zp);
        particle->setInitialVelocity(Xv, Yv, Zv);

        index = i * DefaultParameters::OCLOffset;

     /*** OpenCL vector ***/
        // Position
        clInput[index] = Xp;
        clInput[index+1] = Yp;
        clInput[index+2] = Zp;

        // Velocity
        clInput[index+3] = Xv;
        clInput[index+4] = Yv;
        clInput[index+5] = Zv;

        //Density and pressure
        clInput[index+6] = 0;
        clInput[index+7] = 0;
     /*** end OpenCL init ***/

        particle->reset();

        this->addItem(particle);
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

float ParticleSimulator::getReferenceDensity() const
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

void ParticleSimulator::setReferenceDensity(const double & coeff_rho0) throw(std::invalid_argument)
{
    if(coeff_rho0 <= 0)
        throw std::invalid_argument("ParticleSimulator::setReferenceDensity: negative of null value given for density");

    this->_coeff_rho0 = coeff_rho0;
}

void ParticleSimulator::setParticlesMass(const double & mass) throw(std::invalid_argument)
{
    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        Particle * particle = dynamic_cast<Particle*>(_items[i]);

        particle->setMass(mass);
    }
}

void ParticleSimulator::setOpenClContext(const unsigned int & workSize, QCLContext * openClContext,
                                         QCLVector<float> * openClInput,
                                         QCLVector<float> * openClOutput) throw(std::runtime_error)
{
    _openClContext = openClContext;
    _openClInput = openClInput;
    _openClOutput = openClOutput;

    _openClProgram = _openClContext->buildProgramFromSourceFile("./gpu_main_barrier.c");

    _openClTranslationKernel = _openClProgram.createKernel("gpu_step");

    _openClTranslationKernel.setGlobalWorkSize(workSize);
}

void ParticleSimulator::setGPUMode(const bool & newMode) throw(std::logic_error)
{
    if(newMode && (_openClContext==NULL || _openClInput==NULL))
    {
        throw std::logic_error("ParticleSimulator::setGPUMode: attempt to set GPU computation mode while OpenCl context not set.\n\t Consider using wlSimulator::setOpenClContext");
    }

    // We don't wan to update CLInputVector if
    // computation mode didn't actually changed
    bool previousMode = _gpuMode;
    if(previousMode != newMode)
    {
        _gpuMode = newMode;

        if(_gpuMode)
            _updateCLVector(*_openClInput);
    }
}

void ParticleSimulator::_updateCLVector(QCLVector<float> & openClVector)
{
    unsigned int index;
    Particle * particle;

    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);
        if(particle)
        {
            index = i * DefaultParameters::OCLOffset;

            // Position
            QVector<float> pos = particle->getPosition();
            openClVector[index] = pos[0];
            openClVector[index+1] = pos[1];
            openClVector[index+2] = pos[2];

            // Velocity
            QVector<float> vel = particle->getVelocity();
            openClVector[index+3] = vel[0];
            openClVector[index+4] = vel[1];
            openClVector[index+5] = vel[2];

            //Density and pressure
            openClVector[index+6] = particle->getDensity();
            openClVector[index+7] = particle->getPressure();
        }
    }
}

void ParticleSimulator::printSelf()
{
    Simulator::printSelf();

    std::cout << "ParticleSimulator::PrintSelf" << std::endl;
    if(_gpuMode)
        std::cout << "Mode de calcul : GPU" << std::endl;
    else
        std::cout << "Mode de calcul : CPU" << std::endl;

    std::cout << "Les constantes de la simulation : "
              << "d=" << _coeff_d << ", k=" <<_coeff_k << ", mhu=" <<_coeff_mu
              << ", rho_0=" <<_coeff_rho0 << "" << std::endl;
}

void ParticleSimulator::printParticles() const
{
    Particle * particle;
    for(unsigned int i = 0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            particle->printSelf();
            std::cout << std::endl;
        }
    }
}

void ParticleSimulator::printCLVectors() const
{
    QCLVector<float> & openClInput = *_openClInput;
    QCLVector<float> & openClOutput = *_openClOutput;
    int index;
    std::cout << "Content of openCLInput:" << std::endl;
    for(unsigned int i = 0; i < (unsigned int)_items.size(); ++i)
    {
        index = i * DefaultParameters::OCLOffset;
        std::cout << "("
                  << openClInput[index] << ", "
                  << openClInput[index+1] << ", "
                  << openClInput[index+2] << ", "
                  << openClInput[index+3] << ", "
                  << openClInput[index+4] << ", "
                  << openClInput[index+5] << ", "
                  << openClInput[index+6] << ", "
                  << openClInput[index+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Content of openCLOutput:" << std::endl;
    for(unsigned int i = 0; i < (unsigned int)_items.size(); ++i)
    {
        index = i * DefaultParameters::OCLOffset;
        std::cout << "("
                  << openClOutput[index] << ", "
                  << openClOutput[index+1] << ", "
                  << openClOutput[index+2] << ", "
                  << openClOutput[index+3] << ", "
                  << openClOutput[index+4] << ", "
                  << openClOutput[index+5] << ", "
                  << openClOutput[index+6] << ", "
                  << openClOutput[index+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;
}

void ParticleSimulator::_fetchResults(const QCLVector<float> & openClVector)
{
    Particle * particle;
    unsigned int index;
    for(unsigned int i = 0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            index = i * DefaultParameters::OCLOffset;
            particle->setPosition(openClVector[index], openClVector[index+1], openClVector[index+2]);
            particle->setVelocity(openClVector[index+3], openClVector[index+4], openClVector[index+5]);
            particle->setDensity(openClVector[index+6]);
            particle->setPressure(openClVector[index+7]);

            particle->newStep();
        }
    }
}

void ParticleSimulator::_swapCLVectors()
{
    QCLVector<float> * temp;

    temp = _openClInput;
    _openClInput = _openClOutput;
    _openClOutput = temp;
}

void ParticleSimulator::_setKernelArgs(QCLKernel & kernel)
{
    float particleMass;
    Particle * particle = (!_items.empty()) ? dynamic_cast<Particle*>(_items[0]) : NULL;
    particleMass = (particle != NULL) ? particle->getMass() : 0.;

    kernel.setArg(0, *_openClInput);
    kernel.setArg(1, *_openClOutput);

    kernel.setArg(2, _env.getXMin());
    kernel.setArg(3, _env.getXMax());
    kernel.setArg(4, _env.getYMin());
    kernel.setArg(5, _env.getYMax());
    kernel.setArg(6, _env.getZMin());
    kernel.setArg(7, _env.getZMax());
    kernel.setArg(8, _items.size());
    kernel.setArg(9, _cstep);
    kernel.setArg(10, _timestep);
    kernel.setArg(11, particleMass);
    kernel.setArg(12, _coeff_d);
    kernel.setArg(13, _coeff_k);
    kernel.setArg(14, _coeff_mu);
    kernel.setArg(15, _coeff_rho0);
}

void ParticleSimulator::_gpuStep()
{
    _setKernelArgs(_openClTranslationKernel);

    _fetchResults(*_openClOutput);
    _swapCLVectors();
}


void ParticleSimulator::_cpuStep()
{
    Particle * particle;

    //Compute densities first
    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            particle->computeDensity(_coeff_d, this->_coeff_rho0, this->_coeff_k);
        }

    }

    //compute velocities now
    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            particle->computeTranslation(_coeff_d, this->_coeff_mu);
        }
    }
}

void ParticleSimulator::step()
{
    if(_gpuMode)
    {
        _gpuStep();
    }

    else
    {
        _cpuStep();
    }

    ++_cstep;

    if(_cstep > _nsteps)
        _timer->stop();

    //this->draw();
    emit requestUpdateGL();
}

void ParticleSimulator::restart()
{
    Simulator::restart();
    if(_gpuMode)
        _updateCLVector(*_openClInput);
}

void ParticleSimulator::reset()
{
    _coeff_d = DefaultParameters::Coeff_d;
    _coeff_k = DefaultParameters::Coeff_k;
    _coeff_mu = DefaultParameters::Coeff_mu;
    _coeff_rho0 = DefaultParameters::Rho0;

    Simulator::reset();

    if(_gpuMode)
        _updateCLVector(*_openClInput);

    this->draw();
    emit requestUpdateGL();
}
