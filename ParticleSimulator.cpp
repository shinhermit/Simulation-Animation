#include "ParticleSimulator.h"

ParticleSimulator::ParticleSimulator(QGLViewer *viewer)
    :_viewer(viewer)
{
    _clear();
    ::srand(::time(NULL));

    if (_viewer != NULL)
        connect(_viewer, SIGNAL(drawNeeded()), this, SLOT(draw()));

    _timer = new QTimer(this);
    _timer->setInterval((int)(_timestep*1000));
    connect(_timer, SIGNAL(timeout()), this, SLOT(step()));
    _first = true;
}

ParticleSimulator::~ParticleSimulator()
{
    if(_timer->isActive())
        _timer->stop();
    delete(_timer);

    _clInput_p = NULL;
    _clInput_p = NULL;
    _clInput.release();
    _clOutput.release();
    _clProgram.unloadCompiler();
    _clContext.release();
}



void ParticleSimulator::_clear()
{
    _nsteps = DefaultParameters::NbSteps;
    _timestep = DefaultParameters::TimeStep;
    _cstep = 0;

    _gpuMode = DefaultParameters::GpuMode;
    _coeff_d = DefaultParameters::Coeff_d;
    _coeff_k = DefaultParameters::Coeff_k;
    _coeff_mu = DefaultParameters::Coeff_mu;
    _coeff_rho0 = DefaultParameters::Rho0;
    _particleMass = DefaultParameters::Mass;

    _clInput_p = &_clInput;
    _clOutput_p = &_clOutput;
}

void ParticleSimulator::createParticles()
{
    float Xp, Yp, Zp, Xv, Yv, Zv, zOffset;
    float xMin, xMax, yMin, yMax, zMin, zMax;
    float vMin, vMax;
    float padding;
    int i;

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

    for(i=0; i < _clInput.size(); i+=DefaultParameters::CLOffset)
    {
        Xp = xMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(xMax-xMin));
        Yp = yMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(yMax-yMin));
        Zp = zMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(zMax-zMin));

        Xv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));
        Yv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));
        Zv = vMin + static_cast<float>(::rand()) / static_cast<float>(1.*RAND_MAX/(vMax-vMin));

     /*** OpenCL vector ***/
        // Position
        _initial[i] = Xp;
        _clInput[i] = Xp;

        _initial[i+1] = Yp;
        _clInput[i+1] = Yp;

        _initial[i+2] = Zp;
        _clInput[i+2] = Zp;

        // Velocity
        _initial[i+3] = Xv;
        _clInput[i+3] = Xv;

        _initial[i+4] = Yv;
        _clInput[i+4] = Yv;

        _initial[i+5] = Zv;
        _clInput[i+5] = Zv;

        //Density and pressure
        _initial[i+6] = DefaultParameters::Density;
        _clInput[i+6] = DefaultParameters::Density;

        _initial[i+7] = DefaultParameters::Pressure;
        _clInput[i+7] = DefaultParameters::Pressure;
     /*** end OpenCL init ***/
    }
    std::cout << "ParticleSimulator::createParticles : last index is " << i << std::endl;
}

bool ParticleSimulator::isGPUMode()const
{
    return _gpuMode;
}

float ParticleSimulator::getPressureTolerance()const
{
    return _coeff_d;
}

float ParticleSimulator::getPressureToDensityGradientProportionnality()const
{
    return _coeff_k;
}

float ParticleSimulator::getDynamicViscosityConstant()const
{
    return _coeff_mu;
}

float ParticleSimulator::getReferenceDensity() const
{
    return _coeff_rho0;
}

int ParticleSimulator::size() const
{
    return _clInput.size() / DefaultParameters::CLOffset;
}

int ParticleSimulator::clVectorsSize() const
{
    return _clInput.size();
}

void ParticleSimulator::setSmoothingTolerance(const double & coeff_d) throw(std::invalid_argument)
{
    if(coeff_d <= 0)
        throw std::invalid_argument("ParticleSimulator::setPressureTolerance: negative of null value given for distance tolerance");

    _coeff_d = coeff_d;
}

void ParticleSimulator::setPressureToDensityGradientProportionnality(const double & coeff_k)
{
    _coeff_k = coeff_k;
}

void ParticleSimulator::setDynamicViscosityConstant(const double & coeff_mu)
{
    _coeff_mu = coeff_mu;
}

void ParticleSimulator::setReferenceDensity(const double & coeff_rho0) throw(std::invalid_argument)
{
    if(coeff_rho0 <= 0)
        throw std::invalid_argument("ParticleSimulator::setReferenceDensity: negative of null value given for density");

    _coeff_rho0 = coeff_rho0;
}

void ParticleSimulator::setParticlesMass(const double & mass) throw(std::invalid_argument)
{
    if(mass <= 0)
        throw std::invalid_argument("ParticleSimulator::setReferenceDensity: negative of null value given for density");

    _particleMass = mass;
}

void ParticleSimulator::setOpenClContext(const int & nbParticle) throw(std::runtime_error)
{
    int clSize = DefaultParameters::CLOffset * nbParticle;

    if (!_clContext.create())
    {
        std::cerr << "Could not create OpenCL context for the GPU\n" << std::endl;
        throw std::runtime_error("Project::_configOpenCL: Could not create OpenCL context for the GPU\n");
    }

    _initial.resize(clSize);
    _clInput = _clContext.createVector<float>(clSize);
    _clOutput = _clContext.createVector<float>(clSize);
    _clInput_p = &_clInput;
    _clOutput_p = &_clOutput;

    _clProgram = _clContext.buildProgramFromSourceFile("./gpu_main_barrier.c");

    _clKernel = _clProgram.createKernel("gpu_step");

    _clKernel.setGlobalWorkSize(nbParticle);
}

void ParticleSimulator::setGPUMode(const bool & newMode) throw(std::logic_error)
{
//    if(newMode && (!_clContext.isCreated() || _clInput.isNull()))
//        throw std::logic_error("ParticleSimulator::setGPUMode: attempt to set GPU computation mode while OpenCl context not set.\n\t Consider using wlSimulator::setOpenClContext");

    _gpuMode = newMode;
}

void ParticleSimulator::setTimeStep(const double & timestep)
{
    if (timestep > 0)
    {
        _timestep = timestep;
        _timer->setInterval((int)(timestep*1000));
    }
}

void ParticleSimulator::setNumberOfTimeSteps(const int & nbSteps)
{
    if (nbSteps > 0)
    {
        _nsteps = nbSteps;
    }
}

void ParticleSimulator::printSelf()
{
    std::cout << "Simulation en cours :";
    std::cout << "  Pas de temps : " << _timestep;
    std::cout << "  Nombre de pas de temps : " << _nsteps;
    std::cout << "  Pas de temps courant : " << _cstep;

    std::cout << "ParticleSimulator::PrintSelf" << std::endl;
    if(_gpuMode)
        std::cout << "Mode de calcul : GPU" << std::endl;
    else
        std::cout << "Mode de calcul : CPU" << std::endl;
    std::cout << "Les constantes de la simulation : "
              << "d=" << _coeff_d
              << ", k=" << _coeff_k
              << ", mhu=" << _coeff_mu
              << ", rho_0=" << _coeff_rho0
              << ", mass=" << _particleMass
              << std::endl;

    std::cout << "Les particules:";
    this->printParticles();
}

void ParticleSimulator::printParticles() const
{
    QCLVector<float> & clInput = *_clInput_p;
    for(int i = 0; i < _clInput.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "  Particle :" << i << std::endl;
        std::cout << "Initial Position : ("<< _initial[i] << ", "<< _initial[i+1] << ", " << _initial[i+2] << ")" << std::endl;
        std::cout << "INitial Speed : ("<< _initial[i+3] << ", "<< _initial[i+4] << ", "<< _initial[i+5] << ")" << std::endl;
        std::cout << "Current Potision : ("<< clInput[i] << ", "<< clInput[i+1] << ", "<< clInput[i+2] << ")" << std::endl;
        std::cout << "Current Speed : ("<< clInput[i+3] << ", "<< clInput[i+4] << ", "<< clInput[i+5] << ")" << std::endl;

        std::cout << "Density : " << clInput[i+6] << std::endl;
        std::cout << "Pressure: " << clInput[i+7] << std::endl;
    }
}

void ParticleSimulator::printCLVectors() const
{
    std::cout << "Content of CLInput:" << std::endl;
    for(int i = 0; i < _clInput.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "("
                  << _clInput[i] << ", "
                  << _clInput[i+1] << ", "
                  << _clInput[i+2] << ", "
                  << _clInput[i+3] << ", "
                  << _clInput[i+4] << ", "
                  << _clInput[i+5] << ", "
                  << _clInput[i+6] << ", "
                  << _clInput[i+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Content of CLOutput:" << std::endl;
    for(int i = 0; i < _clOutput.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "("
                  << _clOutput[i] << ", "
                  << _clOutput[i+1] << ", "
                  << _clOutput[i+2] << ", "
                  << _clOutput[i+3] << ", "
                  << _clOutput[i+4] << ", "
                  << _clOutput[i+5] << ", "
                  << _clOutput[i+6] << ", "
                  << _clOutput[i+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;
}

void ParticleSimulator::_swapCLVectors()
{
    QCLVector<float> * temp;

    temp = _clInput_p;
    _clInput_p = _clOutput_p;
    _clOutput_p = temp;
}

void ParticleSimulator::_setKernelArgs(QCLKernel & kernel)
{
    kernel.setArg(0, *_clInput_p);
    kernel.setArg(1, *_clOutput_p);

    kernel.setArg(2, _env.getXMin());
    kernel.setArg(3, _env.getXMax());
    kernel.setArg(4, _env.getYMin());
    kernel.setArg(5, _env.getYMax());
    kernel.setArg(6, _env.getZMin());
    kernel.setArg(7, _env.getZMax());
    kernel.setArg(8, this->size());
    kernel.setArg(9, _cstep);
    kernel.setArg(10, _timestep);
    kernel.setArg(11, _particleMass);
    kernel.setArg(12, _coeff_d);
    kernel.setArg(13, _coeff_k);
    kernel.setArg(14, _coeff_mu);
    kernel.setArg(15, _coeff_rho0);
}

void ParticleSimulator::_gpuStep()
{
    _setKernelArgs(_clKernel);
    _clKernel.run();
}

void ParticleSimulator::_computeDensities()
{
    float density;
    int i,j;
    QVector<float> R_ij(3);

    QCLVector<float> & clInput = *_clInput_p;
    QCLVector<float> & clOutput = *_clOutput_p;

    for(i=0; i < clInput.size(); i+=DefaultParameters::CLOffset)
    {
        density = 0;
        for(j=0; j < clInput.size(); j+=DefaultParameters::CLOffset)
        {
            if(i != j)
            {
                R_ij[0] = clInput[i] - clInput[j];
                R_ij[1] = clInput[i+1] - clInput[j+1];
                R_ij[2] = clInput[i+2] - clInput[j+2];

                density += _particleMass * SPHKernels::poly6(_coeff_d, R_ij);
            }
        }

        clOutput[i+6] = density;
        clOutput[i+7] = _coeff_k*(density - _coeff_rho0);
    }
    std::cout << "ParticleSimulator::_computeDensities : last indexes are " << i << " " << j << std::endl;
}

void ParticleSimulator::_computeInfluences(const int & i, QVector<float> & gradPressure,
                                           QVector<float> & speedLaplacian)
{
    QVector<float> gradKernel;
    QVector<float> R_ij(3);
    float coeff;
    int j;

    QCLVector<float> & clInput = *_clInput_p;
    QCLVector<float> & clOutput = *_clOutput_p;

    gradPressure << 0 << 0 << 0;
    speedLaplacian << 0 << 0 << 0;

    for(j=0; j < clInput.size(); j+=DefaultParameters::CLOffset)
    {
        R_ij[0] = clInput[i] - clInput[j];
        R_ij[1] = clInput[i+1] - clInput[j+1];
        R_ij[2] = clInput[i+2] - clInput[j+2];

        // Gradient de la pression
        coeff = 0;
        if(clOutput[j+6] != 0)
            coeff = _particleMass * (clOutput[i+7] + clOutput[j+7]) / (2*clOutput[j+6]);
        gradKernel = SPHKernels::spiky(_coeff_d, R_ij);

        gradPressure[0] += coeff*gradKernel[0];
        gradPressure[1] += coeff*gradKernel[1];
        gradPressure[2] += coeff*gradKernel[2];

        //Laplacien de la vitesse
        coeff = 0;
        if(clOutput[j+6] != 0)
            coeff = _particleMass * SPHKernels::visco(_coeff_d, R_ij) / clOutput[j+6];

        speedLaplacian[0] += coeff*(clInput[i+3] - clInput[j+3]);
        speedLaplacian[1] += coeff*(clInput[i+4] - clInput[j+4]);
        speedLaplacian[2] += coeff*(clInput[i+5] - clInput[j+5]);
    }
    std::cout << "ParticleSimulator::_computeInfluences : processing particle " << i << " last indexe is " << " " << j << std::endl;
}

void ParticleSimulator::_computeSmoothing(const int & i, const QVector<float> & gradPressure,
                                          const QVector<float> & speedLaplacian)
{
    QVector<float> acc;
    float xCstrt, yCstrt, zCstrt;

    QCLVector<float> & clInput = *_clInput_p;
    QCLVector<float> & clOutput = *_clOutput_p;

    // The gravity
    acc << 0 << 0 << -9.8;

    // Environment constraints (limits)
    xCstrt = (clInput[i] <= _env.getXMin() || clInput[i]>= _env.getXMax()) ? -1 : 1;
    yCstrt = (clInput[i+1] <= _env.getYMin() || clInput[i+1]>= _env.getYMax()) ? -1 : 1;
    zCstrt = (clInput[i+2] <= _env.getZMin()) ? -1 : 1;

    // The influences
    if(clOutput[i+6] != 0)
    {
        acc[0] += xCstrt *( (_coeff_mu*speedLaplacian[0] - gradPressure[0])/clOutput[i+6] );
        acc[1] += yCstrt *( (_coeff_mu*speedLaplacian[1] - gradPressure[1])/clOutput[i+6] );
        acc[2] += zCstrt *( (_coeff_mu*speedLaplacian[2] - gradPressure[2])/clOutput[i+6] );
    }

    //La vitesse
    QVector<float> v0;
    v0 << clInput[i+3] << clInput[i+4] << clInput[i+5]; //Save v_0

    clOutput[i+3] = xCstrt*( v0[0] + acc[0]*_timestep );
    clOutput[i+4] = yCstrt*( v0[1] + acc[1]*_timestep );
    clOutput[i+5] = zCstrt*( v0[2] + acc[2]*_timestep );

    //La translation
    // x = 1/2*a*t^2 + v_0*t + x_0
    // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
    float time = _timestep * (_cstep+1);
    float ptime = time - _timestep;
    std::cout << "ParticleSimulator::_computeSmoothing : processing particle " << i << std::endl;
    clOutput[i] = clInput[i] + xCstrt*( 0.5*acc[0]*(time*time - ptime*ptime) + v0[0]*_timestep );
    clOutput[i+1] = clInput[i+1] + yCstrt*( 0.5*acc[1]*(time*time - ptime*ptime) + v0[1]*_timestep );
    std::cout << "toto" << std::endl;
//    clOutput[i+2] = clInput[i+2] + zCstrt*( 0.5*acc[2]*(time*time - ptime*ptime) + v0[2]*_timestep );
    clOutput[i+2] = 1;
    std::cout << "titi" << std::endl;
}

void ParticleSimulator::_computePositions()
{
    QCLVector<float> & clInput = *_clInput_p;
    QVector<float> gradP, laplV;

    for(int i=0; i < clInput.size(); i+=DefaultParameters::CLOffset)
    {
        _computeInfluences(i, gradP, laplV);
        _computeSmoothing(i, gradP, laplV);
    }

}

void ParticleSimulator::_cpuStep()
{
    _computeDensities();
    _computePositions();
}

void ParticleSimulator::_computeSmoothingDistance()
{
    QVector<float> pos(3);
    QVector<float> min, max;
    float diameter;

    QCLVector<float> & clInput = *_clInput_p;
    if(clInput.size() >= DefaultParameters::CLOffset)
    {
        min << clInput[0] << clInput[1] << clInput[2];
        max = min;
        for(int i=DefaultParameters::CLOffset; i < clInput.size(); i+=DefaultParameters::CLOffset)
        {
            pos[0] = clInput[i];
            pos[1] = clInput[i+1];
            pos[2] = clInput[i+2];

            if(pos[0] < min[0] && pos[1] < min[1] && pos[2] < min[2])
                min = pos;
            else if(pos[0] > max[0] && pos[1] > max[1] && pos[2] > max[2])
                max = pos;
        }

        diameter = ::sqrt((max[0]-min[0])*(max[0]-min[0]) + (max[1]-min[1])*(max[1]-min[1]) + (max[2]-min[2])*(max[2]-min[2]));
        _coeff_d = 10*diameter / this->size();

        emit smoothingDistanceChanged(_coeff_d);
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

    _swapCLVectors();

    if(_cstep > _nsteps)
        _timer->stop();

    //_computeSmoothingDistance(); /* Testing dynamic smoothing length */
    emit requestUpdateGL();
}

void ParticleSimulator::restart()
{
    _cstep = 0;

    for(int i = 0; i < _clInput.size(); ++i)
    {
        _clInput[i] = _initial[i];
        _clOutput[i] =_initial[i];
    }

    this->draw();
    emit requestUpdateGL();
}

void ParticleSimulator::reset()
{
    _nsteps = DefaultParameters::NbSteps;
    _timestep = DefaultParameters::TimeStep;
    _cstep = 0;

    _coeff_d = DefaultParameters::Coeff_d;
    _coeff_k = DefaultParameters::Coeff_k;
    _coeff_mu = DefaultParameters::Coeff_mu;
    _coeff_rho0 = DefaultParameters::Rho0;

    for(int i = 0; i < _clInput.size(); ++i)
    {
        _clInput[i] = _initial[i];
        _clOutput[i] =_initial[i];
    }

    this->draw();
    emit requestUpdateGL();
}

void ParticleSimulator::play()
{
    _timer->start();
}

void ParticleSimulator::stop()
{
    _timer->stop();
}

void ParticleSimulator::_setupScene()
{
    QVector<float> ext;
    std::vector<float> size;
    std::vector<float> center;

    ext = _env.getLimits();

    size.push_back(ext[1] - ext[0]);
    size.push_back(ext[3] - ext[2]);
    size.push_back(ext[5] - ext[4]);

    center.push_back(ext[0] + size[0] / 2);
    center.push_back(ext[2] + size[1] / 2);
    center.push_back(ext[4] + size[2] / 2);

    float radius = (float)sqrt((double)(size[0] * size[0] + size[1] * size[1] + size[2] * size[2])) / 2;

    _viewer->setSceneRadius(radius*2);
    _viewer->setSceneCenter(qglviewer::Vec(center[0], center[1], 0));
    _viewer->camera()->setRevolveAroundPoint(_viewer->sceneCenter());

    // on se place en [0, -y, 0]
    _viewer->camera()->setPosition(qglviewer::Vec(center[0], center[1]-radius*10, center[2]));
    _viewer->camera()->setViewDirection(_viewer->sceneCenter() - _viewer->camera()->position());
    _viewer->camera()->setUpVector(qglviewer::Vec(0, 0, 1));
    _viewer->camera()->setFieldOfView((float)atan((double)0.2));
    _viewer->camera()->showEntireScene();
}

void ParticleSimulator::draw()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _env.draw();

    QCLVector<float> & clInput = *_clInput_p;

    glPushMatrix();
    glColor3f(0.6, 0.6, 0.6);
    glPointSize(2.);
    glBegin(GL_POINTS);
    for (int i=0 ; i < clInput.size() ; i+=DefaultParameters::CLOffset)
    {
        glVertex3f(clInput[i], clInput[i+1], clInput[i+2]);
    }
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHT0);
    float light_position[] = { 0, -100, 100, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if (_first)
    {
        _setupScene();
        _first = false;
    }

    glFlush();
}
