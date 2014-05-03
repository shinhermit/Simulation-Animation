#ifndef DEFAULTPARAMETERS_H
#define DEFAULTPARAMETERS_H

/// \brief Holds the default parameters and constants of the simulation
class DefaultParameters
{
public:
    static const float Gravity; /*!< The acceleration of the gravity */
    static const short CLOffset; /*!< The offset in the vectors of dynamic properties */

    static const int WindowSize[2]; /*!< width and length of the view */
    static const float EnvironmentLimits[6]; /*!< Default boundaries of the simulation */

    static const int NbSteps; /*!< Number of steps of the simulation */
    static const unsigned int NbParticles; /*!< Number of particles */
    static const double TimeStep; /*!< Duration of one step */

    static const bool GpuMode; /*!< Default Computation mode */

    static const double Coeff_d; /*!< The SPH smoothing distance */
    static const double Coeff_k; /*!< The proportionality ratio of pressure ot density */
    static const double Coeff_mu; /*!< The dynamic viscosity constant */
    static const double Rho0; /*!< The reference density */
    static const double Mass; /*!< The default uniform mass of particles */
    static const double Density; /*!< The Default value of the density of a particle */
    static const double Pressure; /*!< The default value of the pressure of a particle */
};

#endif // DEFAULTPARAMETERS_H
