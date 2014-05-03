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

    _input_p = &_input;
    _output_p = &_output;

    _openClContext = NULL;
    _openClInput = NULL;
    _openClOutput = NULL;
}

void ParticleSimulator::initialize(const unsigned int &nbParticle,
                                   QCLContext *openClContext,
                                   QCLVector<float> *openClInput,
                                   QCLVector<float> *openClOutput) throw(std::runtime_error)
{
    unsigned int clSize = DefaultParameters::CLOffset * nbParticle;

    _initial.resize(clSize);

    _input.resize(clSize);
    _output.resize(clSize);
    _input_p = &_input;
    _output_p = &_output;

    _openClContext = openClContext;
    _openClInput = openClInput;
    _openClOutput = openClOutput;

    _openClProgram = _openClContext->buildProgramFromSourceFile("./gpu_main_barrier.c");
    _openClTranslationKernel = _openClProgram.createKernel("gpu_step");
    _openClTranslationKernel.setGlobalWorkSize(nbParticle);
}

void ParticleSimulator::createParticles(const unsigned int & nbItems)
{
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

        index = i * DefaultParameters::CLOffset;

     /*** OpenCL vector ***/
        // Position
        _initial[index] = Xp;
        _input[index] = Xp;
        clInput[index] = Xp;

        _initial[index+1] = Yp;
        _input[index+1] = Yp;
        clInput[index+1] = Yp;

        _initial[index+2] = Zp;
        _input[index+2] = Zp;
        clInput[index+2] = Zp;

        // Velocity
        _initial[index+3] = Xv;
        _input[index+3] = Xv;
        clInput[index+3] = Xv;

        _initial[index+4] = Yv;
        _input[index+4] = Yv;
        clInput[index+4] = Yv;

        _initial[index+5] = Zv;
        _input[index+5] = Zv;
        clInput[index+5] = Zv;

        //Density and pressure
        _initial[index+6] = DefaultParameters::Density;
        _input[index+6] = DefaultParameters::Density;
        clInput[index+6] = DefaultParameters::Density;

        _initial[index+7] = DefaultParameters::Pressure;
        _input[index+7] = DefaultParameters::Pressure;
        clInput[index+7] = DefaultParameters::Pressure;
     /*** end OpenCL init ***/
    }
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
    return _input.size() / DefaultParameters::CLOffset;
}

int ParticleSimulator::clVectorsSize() const
{
    return _input.size();
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

void ParticleSimulator::setGPUMode(const bool & newMode) throw(std::logic_error)
{
    if(newMode && (_openClContext==NULL || _openClInput==NULL))
    {
        throw std::logic_error("ParticleSimulator::setGPUMode: attempt to set GPU computation mode while OpenCl context not set.\n\t Consider using wlSimulator::setOpenClContext");
    }


    if(_gpuMode != newMode)
    {
        _gpuMode = newMode;
        if(_gpuMode)
            _updateCLInput();
        else
            _fetchCLResults();
    }
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

void ParticleSimulator::printParticles()
{
    if(_gpuMode)
        _fetchCLResults();

    QVector<float> & input = *_input_p;
    for(int i = 0; i < input.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "  Particle :" << i << std::endl;
        std::cout << "Initial Position : ("<< _initial[i] << ", "<< _initial[i+1] << ", " << _initial[i+2] << ")" << std::endl;
        std::cout << "INitial Speed : ("<< _initial[i+3] << ", "<< _initial[i+4] << ", "<< _initial[i+5] << ")" << std::endl;
        std::cout << "Current Potision : ("<< input[i] << ", "<< input[i+1] << ", "<< input[i+2] << ")" << std::endl;
        std::cout << "Current Speed : ("<< input[i+3] << ", "<< input[i+4] << ", "<< input[i+5] << ")" << std::endl;

        std::cout << "Density : " << input[i+6] << std::endl;
        std::cout << "Pressure: " << input[i+7] << std::endl;
    }
}

void ParticleSimulator::printCLVectors()
{
    if(_gpuMode)
        _fetchCLResults();

    std::cout << "Content of input:" << std::endl;
    for(int i = 0; i < _input.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "("
                  << _input[i] << ", "
                  << _input[i+1] << ", "
                  << _input[i+2] << ", "
                  << _input[i+3] << ", "
                  << _input[i+4] << ", "
                  << _input[i+5] << ", "
                  << _input[i+6] << ", "
                  << _input[i+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Content of output:" << std::endl;
    for(int i = 0; i < _output.size(); i+=DefaultParameters::CLOffset)
    {
        std::cout << "("
                  << _output[i] << ", "
                  << _output[i+1] << ", "
                  << _output[i+2] << ", "
                  << _output[i+3] << ", "
                  << _output[i+4] << ", "
                  << _output[i+5] << ", "
                  << _output[i+6] << ", "
                  << _output[i+7] << ", "
                  << ")" << std::endl;
    }
    std::cout << std::endl;
}

void ParticleSimulator::_swapVectors()
{
    if(_gpuMode)
    {
        QCLVector<float> * temp;

        temp = _openClInput;
        _openClInput = _openClOutput;
        _openClOutput = temp;
    }
    else
    {
        QVector<float> * temp;

        temp = _input_p;
        _input_p = _output_p;
        _output_p = temp;
    }
}

void ParticleSimulator::_fetchCLResults()
{
    QCLVector<float> & clOutput = *_openClOutput;
    QVector<float> & output = *_output_p;
    for(int i=0; i < clOutput.size(); ++i)
    {
        output[i] = clOutput[i];
    }
}

void ParticleSimulator::_updateCLInput()
{
    QCLVector<float> & clInput = *_openClInput;
    QVector<float> & input = *_input_p;
    for(int i=0; i < clInput.size(); ++i)
    {
         clInput[i] = input[i];
    }
}

void ParticleSimulator::_setKernelArgs(QCLKernel & kernel)
{
    kernel.setArg(0, *_openClInput);
    kernel.setArg(1, *_openClOutput);

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
    _setKernelArgs(_openClTranslationKernel);
    _openClTranslationKernel.run();
}

void ParticleSimulator::_computeDensities()
{
    float density;
    int i,j;
    QVector<float> R_ij(3);

    QVector<float> & input = *_input_p;
    QVector<float> & output = *_output_p;

    for(i=0; i < input.size(); i+=DefaultParameters::CLOffset)
    {
        density = 0;
        for(j=0; j < input.size(); j+=DefaultParameters::CLOffset)
        {
            if(i != j)
            {
                R_ij[0] = input[i] - input[j];
                R_ij[1] = input[i+1] - input[j+1];
                R_ij[2] = input[i+2] - input[j+2];

                density += _particleMass * SPHKernels::poly6(_coeff_d, R_ij);
            }
        }

        output[i+6] = density;
        output[i+7] = _coeff_k*(density - _coeff_rho0);
    }
}

void ParticleSimulator::_computeInfluences(const int & i, QVector<float> & gradPressure,
                                           QVector<float> & speedLaplacian)
{
    QVector<float> gradKernel;
    QVector<float> R_ij(3);
    float coeff;
    int j;

    QVector<float> & input = *_input_p;
    QVector<float> & output = *_output_p;

    gradPressure.resize(3);
    gradPressure.fill(0);
    speedLaplacian.resize(3);
    speedLaplacian.fill(0);

    for(j=0; j < input.size(); j+=DefaultParameters::CLOffset)
    {
        if(j != i)
        {
            R_ij[0] = input[i] - input[j];
            R_ij[1] = input[i+1] - input[j+1];
            R_ij[2] = input[i+2] - input[j+2];

            // Gradient de la pression
            coeff = 0;
            if(output[j+6] != 0)
                coeff = _particleMass * (output[i+7] + output[j+7]) / (2*output[j+6]);
            gradKernel = SPHKernels::spiky(_coeff_d, R_ij);

            gradPressure[0] += coeff*gradKernel[0];
            gradPressure[1] += coeff*gradKernel[1];
            gradPressure[2] += coeff*gradKernel[2];

            //Laplacien de la vitesse
            coeff = 0;
            if(output[j+6] != 0)
                coeff = _particleMass * SPHKernels::visco(_coeff_d, R_ij) / output[j+6];

            speedLaplacian[0] += coeff*(input[i+3] - input[j+3]);
            speedLaplacian[1] += coeff*(input[i+4] - input[j+4]);
            speedLaplacian[2] += coeff*(input[i+5] - input[j+5]);
        }
    }
}

void ParticleSimulator::_computeSmoothing(const int & i, const QVector<float> & gradPressure,
                                          const QVector<float> & speedLaplacian)
{
    QVector<float> acc;
    float xCstrt, yCstrt, zCstrt;

    QVector<float> & input = *_input_p;
    QVector<float> & output = *_output_p;

    // The gravity
    acc << 0 << 0 << -9.8;

    // Environment constraints (limits)
    xCstrt = (input[i] <= _env.getXMin() || input[i]>= _env.getXMax()) ? -1 : 1;
    yCstrt = (input[i+1] <= _env.getYMin() || input[i+1]>= _env.getYMax()) ? -1 : 1;
    zCstrt = (input[i+2] <= _env.getZMin()) ? -1 : 1;

    // The influences
    if(output[i+6] != 0)
    {
        acc[0] += xCstrt *( (_coeff_mu*speedLaplacian[0] - gradPressure[0])/output[i+6] );
        acc[1] += yCstrt *( (_coeff_mu*speedLaplacian[1] - gradPressure[1])/output[i+6] );
        acc[2] += zCstrt *( (_coeff_mu*speedLaplacian[2] - gradPressure[2])/output[i+6] );
    }

    //La vitesse
    QVector<float> v0;
    v0 << input[i+3] << input[i+4] << input[i+5]; //Save v_0

    output[i+3] = xCstrt*( v0[0] + acc[0]*_timestep );
    output[i+4] = yCstrt*( v0[1] + acc[1]*_timestep );
    output[i+5] = zCstrt*( v0[2] + acc[2]*_timestep );

    //La translation
    // x = 1/2*a*t^2 + v_0*t + x_0
    // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
    float time = _timestep * (_cstep+1);
    float ptime = time - _timestep;
    output[i] = input[i] + xCstrt*( 0.5*acc[0]*(time*time - ptime*ptime) + v0[0]*_timestep );
    output[i+1] = input[i+1] + yCstrt*( 0.5*acc[1]*(time*time - ptime*ptime) + v0[1]*_timestep );
    output[i+2] = input[i+2] + zCstrt*( 0.5*acc[2]*(time*time - ptime*ptime) + v0[2]*_timestep );
}

void ParticleSimulator::_computePositions()
{
    QVector<float> & input = *_input_p;
    QVector<float> gradP, laplV;

    for(int i=0; i < input.size(); i+=DefaultParameters::CLOffset)
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
    float diameter, before;

    before = _coeff_d;

    if(_gpuMode)
    {
        QCLVector<float> & input = *_openClInput;
        if(input.size() >= DefaultParameters::CLOffset)
        {
            min << input[0] << input[1] << input[2];
            max = min;
            for(int i=DefaultParameters::CLOffset; i < input.size(); i+=DefaultParameters::CLOffset)
            {
                pos[0] = input[i];
                pos[1] = input[i+1];
                pos[2] = input[i+2];

                if(pos[0] < min[0] && pos[1] < min[1] && pos[2] < min[2])
                    min = pos;
                else if(pos[0] > max[0] && pos[1] > max[1] && pos[2] > max[2])
                    max = pos;
            }

            diameter = ::sqrt((max[0]-min[0])*(max[0]-min[0]) + (max[1]-min[1])*(max[1]-min[1]) + (max[2]-min[2])*(max[2]-min[2]));
            _coeff_d = 10*diameter / this->size();
        }
    }
    else // this is almost exactly the same computation, but couldn't find another way
    {
        QVector<float> & input = *_input_p;
        if(input.size() >= DefaultParameters::CLOffset)
        {
            min << input[0] << input[1] << input[2];
            max = min;
            for(int i=DefaultParameters::CLOffset; i < input.size(); i+=DefaultParameters::CLOffset)
            {
                pos[0] = input[i];
                pos[1] = input[i+1];
                pos[2] = input[i+2];

                if(pos[0] < min[0] && pos[1] < min[1] && pos[2] < min[2])
                    min = pos;
                else if(pos[0] > max[0] && pos[1] > max[1] && pos[2] > max[2])
                    max = pos;
            }

            diameter = ::sqrt((max[0]-min[0])*(max[0]-min[0]) + (max[1]-min[1])*(max[1]-min[1]) + (max[2]-min[2])*(max[2]-min[2]));
            _coeff_d = 10*diameter / this->size();
        }
    }

    if(_coeff_d != before)
        emit smoothingDistanceChanged(_coeff_d);
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

    _swapVectors();

    if(_cstep > _nsteps)
        _timer->stop();

    //_computeSmoothingDistance(); /* Testing dynamic smoothing length */
    emit requestUpdateGL();
}

void ParticleSimulator::restart()
{
    _cstep = 0;

    if(_gpuMode)
    {
        for(int i = 0; i < _input.size(); ++i)
        {
            (*_openClInput)[i] = _initial[i];
            (*_openClOutput)[i] =_initial[i];
        }
    }
    else
    {
        for(int i = 0; i < _input.size(); ++i)
        {
            _input[i] = _initial[i];
            _output[i] =_initial[i];
        }
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


    if(_gpuMode)
    {
        for(int i = 0; i < _openClInput->size(); ++i)
        {
            (*_openClInput)[i] = _initial[i];
            (*_openClOutput)[i] =_initial[i];
        }
    }
    else
    {
        for(int i = 0; i < _input.size(); ++i)
        {
            _input[i] = _initial[i];
            _output[i] =_initial[i];
        }
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

    if(_gpuMode)
    {
        QCLVector<float> & input = *_openClInput;

        glPushMatrix();
        glColor3f(0.6, 0.6, 0.6);
        glPointSize(2.);
        glBegin(GL_POINTS);
        for (int i=0 ; i < input.size() ; i+=DefaultParameters::CLOffset)
        {
            glVertex3f(input[i], input[i+1], input[i+2]);
        }
        glEnd();
        glPopMatrix();

        glEnable(GL_LIGHT0);
        float light_position[] = { 0, -10, 10, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        if (_first)
        {
            _setupScene();
            _first = false;
        }

        glFlush();
    }
    else // Same problem again, almost exactly the sale code
    {
        QVector<float> & input = *_input_p;

        glPushMatrix();
        glColor3f(0.6, 0.6, 0.6);
        glPointSize(2.);
        glBegin(GL_POINTS);
        for (int i=0 ; i < input.size() ; i+=DefaultParameters::CLOffset)
        {
            glVertex3f(input[i], input[i+1], input[i+2]);
        }
        glEnd();
        glPopMatrix();

        glEnable(GL_LIGHT0);
        float light_position[] = { 0, -10, 10, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        if (_first)
        {
            _setupScene();
            _first = false;
        }

        glFlush();
    }
}
