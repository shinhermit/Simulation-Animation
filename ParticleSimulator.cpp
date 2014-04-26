#include "ParticleSimulator.h"

ParticleSimulator::ParticleSimulator(int debug, QGLViewer *viewer, wlMesh *environment)
    :Simulator(debug, viewer, environment, new QVector<AnimatedObject*>())
{
    _clear();
}

void ParticleSimulator::_clear()
{
    Simulator::_clear();

    _gpuMode = false;
    _coeff_d = DefaultParameters::Coeff_d;
    _coeff_k = DefaultParameters::Coeff_k;
    _coeff_mu = DefaultParameters::Coeff_mu;
    _coeff_rho0 = DefaultParameters::Rho0;

    _openClContext = NULL;
    _openClInput = NULL;
    _openClOutput = NULL;
}

void ParticleSimulator::createParticles(const unsigned int & nbItems, const int & debug)
{
    Particle * particle;
    float Xp, Yp, Zp, zOffset;
    unsigned int index;

    double itemsPerSide = ::pow(nbItems, 1./3);
    float step = 1;
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

                particle = new Particle(_items, debug);
                particle->setInitialPosition(Xp, Yp, Zp);
                particle->setInitialVelocity(0, 0, 0);

                if(_gpuMode)
                {
                    QCLVector<float> & clInput = *_openClInput;

                    index = k + j*itemsPerSide + i*itemsPerSide*itemsPerSide;
                    index *= DefaultParameters::OCLOffset;

                    // Position
                    clInput[index] = Xp;
                    clInput[index+1] = Yp;
                    clInput[index+2] = Zp;

                    // Velocity
                    clInput[index+3] = 0;
                    clInput[index+4] = 0;
                    clInput[index+5] = 0;

                    //Density and pressure
                    clInput[index+6] = 0;
                    clInput[index+7] = 0;
                }

                particle->reset();

                this->addItem(particle);
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
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for density");

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
    _gpuMode = true;

    _openClContext = openClContext;
    _openClInput = openClInput;
    _openClOutput = openClOutput;

    _openClProgram = _openClContext->buildProgramFromSourceFile("./gpu_bindel_formula.c");

    _openClTranslationKernel = _openClProgram.createKernel("compute_density");
    _openClDensityKernel = _openClProgram.createKernel("compute_translation");

    _openClTranslationKernel.setGlobalWorkSize(workSize);
    _openClDensityKernel.setGlobalWorkSize(workSize);
}

//void ParticleSimulator::setOpenClContext(QCLContext * openClContext,
//                                         QCLVector<float> * openClInput) throw(std::runtime_error)
//{
//    _gpuMode = true;

//    _openClContext = openClContext;
//    _openClInput = openClInput;

//    _openClProgram = _openClContext->buildProgramFromSourceFile("./gpu_main.c");

//    _openClDensityKernel = _openClProgram.createKernel("compute_density");
//    _openClTranslationKernel = _openClProgram.createKernel("compute_translation");

//    _openClDensityKernel.setGlobalWorkSize(_items.size());
//    _openClTranslationKernel.setGlobalWorkSize(_items.size());

//    // Problem if not set: Floating point exception
//    _openClDensityKernel.setLocalWorkSize(1);
//    _openClTranslationKernel.setLocalWorkSize(1);
//}

void ParticleSimulator::setGPUMode(const bool & gpuMode) throw(std::logic_error)
{
    if(gpuMode && (this->_openClContext==NULL || this->_openClInput==NULL))
    {
        throw std::logic_error("ParticleSimulator::setGPUMode: attempt to set GPU computation mode while OpenCl context not set.\n\t Consider using wlSimulator::setOpenClContext");
    }

    this->_gpuMode = gpuMode;
}

void ParticleSimulator::printSelf()
{
    Simulator::printSelf();

    this->Print("ParticleSimulator::PrintSelf");
    if(_gpuMode)
        this->Print("Mode de calcul : GPU");
    else
        this->Print("Mode de calcul : CPU");
    this->Print("Les constantes de la simulation : d=%f, k=%f, mhu=%f, rho_0=%f", _coeff_d, _coeff_k, _coeff_mu, _coeff_rho0);
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
    std::cout << "Content of opencInput:" << std::endl;
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

    std::cout << "Content of opencOutput:" << std::endl;
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

//void ParticleSimulator::_gpuStep()
//{
//    Particle * particle;
//    QCLVector<float> & openClInput = *_openClInput;
//    unsigned int nbItems = (unsigned int)_items.size();
//    float particleMass;

//    particle = (!_items.empty()) ? dynamic_cast<Particle*>(_items[0]) : NULL;
//    particleMass = (particle != NULL) ? particle->getMass() : 0.;

//    /*__global __read_write float * data, unsigned int nbItems,
//                  float particleMass, float maxDist, float coeff_k, float refDensity*/
//    _openClDensityKernel(openClInput, nbItems, particleMass, _coeff_d, _coeff_k, _coeff_rho0);

//    /*__global __read_write float * data, unsigned int * cstep, float timestep,
//                  unsigned int nbItems, float particleMass, float maxDist, float coeff_mu*/
//    _openClTranslationKernel(openClInput, _cstep, _timestep, nbItems, particleMass, _coeff_d, _coeff_mu);
//}

void ParticleSimulator::_copyResults(const QCLVector<float> & openClVector)
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

void ParticleSimulator::_gpuStep()
{
    Particle * particle;
    unsigned int nbItems = (unsigned int)_items.size();
    float particleMass;

    particle = (!_items.empty()) ? dynamic_cast<Particle*>(_items[0]) : NULL;
    particleMass = (particle != NULL) ? particle->getMass() : 0.;

    /*__global __read_only float * input , __global __write_only float * output,  unsigned int nbItems, unsigned int cstep, float timestep,
                  float particleMass, float maxDist, float coeff_k, float coeff_mu, float refDensity*/
    _openClDensityKernel(*_openClInput, *_openClOutput, nbItems, _cstep, _timestep, particleMass, _coeff_d, _coeff_k, _coeff_mu, _coeff_rho0);
    _openClTranslationKernel(*_openClInput, *_openClOutput, nbItems, _cstep, _timestep, particleMass, _coeff_d, _coeff_k, _coeff_mu, _coeff_rho0);

    _copyResults(*_openClOutput);
    _swapCLVectors();
}


void ParticleSimulator::_cpuStep()
{
    // !! Move the 3 kernels to attributes later !!
    Poly6Kernel poly6(this->_coeff_d);
    SpikyKernel spiky(this->_coeff_d);
    ViscosityKernel viscy(this->_coeff_d);

    Particle * particle;

    //Compute densities first
    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            particle->computeDensity(poly6, this->_coeff_rho0, this->_coeff_k);
        }

    }

    //compute velocities now
    for(unsigned int i=0; i < (unsigned int)_items.size(); ++i)
    {
        particle = dynamic_cast<Particle*>(_items[i]);

        if(particle)
        {
            particle->computeTranslation(spiky, viscy, this->_coeff_mu);
        }
    }
}

void ParticleSimulator::reset()
{
    _clear();
    Simulator::reset();

    this->draw();
    emit requestUpdateGL();
}

void ParticleSimulator::step()
{
    if(this->_gpuMode)
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

    this->draw();
    emit requestUpdateGL();
}
