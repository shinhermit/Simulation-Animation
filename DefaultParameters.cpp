#include "DefaultParameters.h"

const float DefaultParameters::Gravity = -9.8;
const float DefaultParameters::CollisionAttenuation = 0.1;
const float DefaultParameters::EnvironmentPadding = 0.01;
const short DefaultParameters::CLOffset = 8;

const int DefaultParameters::WindowSize[2] = {800, 700};
const float DefaultParameters::EnvironmentLimits[6] = {-1,1,-1,1,-1,1};

const int DefaultParameters::NbSteps = 200;
const unsigned int DefaultParameters::NbParticles = 4000;
const double DefaultParameters::TimeStep = 0.005;
const bool DefaultParameters::GpuMode = true;

const double DefaultParameters::Coeff_d = 0.01;
const double DefaultParameters::Coeff_k = 0.1;
const double DefaultParameters::Coeff_mu = 0.01;
const double DefaultParameters::Rho0 = -1.;
const double DefaultParameters::Mass = 1.;
const double DefaultParameters::Density = 0.;
const double DefaultParameters::Pressure = 0.;
