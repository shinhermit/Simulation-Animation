#include "gpu_sphKernels.h"

__kernel void compute_density(__global __read_write float * data, unsigned int nbItems,
			      float particleMass, float maxDist, float coeff_k, float refDensity)
{

  /***** Identify worker *****/
  unsigned int myId = get_global_id(0);

  /***** Do the work *****/
  float density = 0;
  float R_ij[3];

  unsigned int hisPosIndex;
  unsigned int hisVelIndex;
  unsigned int hisRhoIndex;
  unsigned int hisPresIndex;

  unsigned int myPosIndex = myId*8;
  unsigned int myVelIndex = myPosIndex + 3;
  unsigned int myRhoIndex = myVelIndex + 3;
  unsigned int myPresIndex = myRhoIndex + 1;
  unsigned int i;

  for(i=0; i < nbItems; ++i)
    {
      hisPosIndex = i*8;;
      hisVelIndex = hisPosIndex + 3;
      hisRhoIndex = hisVelIndex + 3;
      hisPresIndex = hisRhoIndex + 1;

      if(i != myId)
        {
	  R_ij[0] = data[myPosIndex] - data[hisPosIndex];
	  R_ij[1] = data[myPosIndex+1] - data[hisPosIndex+1];
	  R_ij[2] = data[myPosIndex+2] - data[hisPosIndex+2];

	  density += particleMass * sph_poly6(maxDist, R_ij);
	}
    }

  data[myRhoIndex] = density;
  data[myPresIndex] = coeff_k*(density - refDensity);
}

__kernel void compute_translation(__global __read_write float * data, unsigned int cstep, float timestep,
				  unsigned int nbItems, float particleMass, float maxDist, float coeff_mu)
{
  /***** Identify worker *****/
  unsigned int myId = get_global_id(0);

  /***** Do the work *****/
  float R_ij[3];
  float gradP[3], laplV[3], gradK[3];
  float acc[3];
  float coeff;

  unsigned int hisPosIndex;
  unsigned int hisVelIndex;
  unsigned int hisRhoIndex;
  unsigned int hisPresIndex;

  unsigned int myPosIndex = myId*8;
  unsigned int myVelIndex = myPosIndex + 3;
  unsigned int myRhoIndex = myVelIndex + 3;
  unsigned int myPresIndex = myRhoIndex + 1;
  unsigned int i;

  gradP[0] = gradP[1] = gradP[2] = 0;
  laplV[0] = laplV[1] = laplV[2] = 0;

  for(i=0; i < nbItems; ++i)
    {
      if(i != myId)
	{
	  hisPosIndex = i*8;
	  hisVelIndex = hisPosIndex + 3;
	  hisRhoIndex = hisVelIndex + 3;
	  hisPresIndex = hisRhoIndex + 1;

	  R_ij[0] = data[myPosIndex] - data[hisPosIndex];
	  R_ij[1] = data[myPosIndex+1] - data[hisPosIndex+1];
	  R_ij[2] = data[myPosIndex+2] - data[hisPosIndex+2];

	  // Gradient de la pression
	  coeff = 0;
	  if(data[hisRhoIndex])
	    coeff = 0.5 * particleMass * (data[myPresIndex] + data[hisPresIndex]) / data[hisRhoIndex];

	  sph_spiky_gradient(maxDist, R_ij, gradK);

	  gradP[0] += coeff*gradK[0];
	  gradP[1] += coeff*gradK[1];
	  gradP[2] += coeff*gradK[2];

	  //Laplacien de la vitesse
	  coeff = 0;
	  if(data[hisRhoIndex])
	    coeff = other->getMass() * kernelV.laplacian(R_ij) / data[hisRhoIndex];

	  laplV[0] += coeff*(data[myVelIndex] - data[hisVelIndex]);
	  laplV[1] += coeff*(data[myVelIndex+1] - data[hisVelIndex+1]);
	  laplV[2] += coeff*(data[myVelIndex+2] - data[hisVelIndex+2]);
	}
    }

  //calcul de l'accélération et de la vitesse
  // The gravity
  acc[0] = acc[1] = 0;
  if(data[myPosIndex+2] > 0.1)
    {
      acc[2] = -9.8;
    }
  else
    {
      acc[2] = -9.8;
      data[myVelIndex] = data[myVelIndex+1] = data[myVelIndex+2] = 0;
    }

  // The influences
  if(data[myRhoIndex])
    {
      acc[0] += (coeff_mu*laplV[0] - gradP[0])/data[myRhoIndex];
      acc[1] += (coeff_mu*laplV[1] - gradP[1])/data[myRhoIndex];
      acc[2] += (coeff_mu*laplV[2] - gradP[2])/data[myRhoIndex];
    }

  ++ cstep;

  //La vitesse
  float v0[3] = {data[myVelIndex], data[myVelIndex+1], data[myVelIndex+2]}; //Save v_0

  data[myVelIndex] += acc[0]*timestep;
  data[myVelIndex+1] += acc[1]*timestep;
  data[myVelIndex+2] += acc[2]*timestep;

  //La translation
  // x = 1/2*a*t^2 + v_0*t + x_0
  // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
  float time = timestep * cstep;
  float time_p = time - timestep;
  data[myPoslIndex] += 0.5*acc[0]*(time*time - time_p*time_p) + v0[0]*timestep;
  data[myPoslIndex+1] += 0.5*acc[1]*(time*time - time_p*time_p) + v0[1]*timestep;
  data[myPoslIndex+2] += 0.5*acc[2]*(time*time - time_p*time_p) + v0[2]*timestep;
}
