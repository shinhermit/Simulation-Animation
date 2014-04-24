#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H

class DefaultParameters
{
public:
    static const float Gravity;
    static const short OCLOffset;
    static const int WindowSize[2];

    static const int NbSteps;
    static const unsigned int NbParticles;
    static const double TimeStep;

    static const double Coeff_d;
    static const double Coeff_k;
    static const double Coeff_mu;
    static const double Rho0;
    static const double Mass;
    static const double Density;
    static const double Pressure;
};

#endif // DEFAULTPARAMETERS_H
