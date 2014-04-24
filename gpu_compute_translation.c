float3 sph_spiky_gradient(float maxDist, float3 R_ij)
{
  float3 res;
  float coef;
  float dist = sqrt(R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z);
  float q = dist/maxDist;

  res.x = res.y = res.z = 0.;

  if(dist <= maxDist && maxDist != 0.)
    {
      coef = -30*pown(1-q, 2) / (3.1415*q*pown(maxDist, 4));

      res.x = coef*R_ij.x;
      res.y = coef*R_ij.y;
      res.z = coef*R_ij.z;
    }

  return res;
}

float sph_viscosity_laplacian(float maxDist, float3 R_ij)
{
  float dist = sqrt(R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z);
  float q = dist/maxDist;

  if(dist <= maxDist && maxDist != 0.)
    {
      return 40*(1 - q) / (3.1415*pown(maxDist, 4));
    }

  else
    return 0.;
}

__kernel void compute_translation(__global float * data, unsigned int cstep, float timestep,
				  unsigned int nbItems, float particleMass, float maxDist, float coeff_mu)
{
  /***** Identify worker *****/
  unsigned int myId = get_global_id(0);

  /***** Do the work *****/
  float3 R_ij;
  float3 gradP, laplV, gradK;
  float3 acc;
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

  gradP.x = gradP.y = gradP.z = 0;
  laplV.x = laplV.y = laplV.z = 0;

  for(i=0; i < nbItems; ++i)
    {
      if(i != myId)
	{
	  hisPosIndex = i*8;
	  hisVelIndex = hisPosIndex + 3;
	  hisRhoIndex = hisVelIndex + 3;
	  hisPresIndex = hisRhoIndex + 1;

	  R_ij.x = data[myPosIndex] - data[hisPosIndex];
	  R_ij.y = data[myPosIndex+1] - data[hisPosIndex+1];
	  R_ij.z = data[myPosIndex+2] - data[hisPosIndex+2];

	  // Gradient de la pression
	  coeff = 0;
	  if(data[hisRhoIndex])
	    coeff = 0.5 * particleMass * (data[myPresIndex] + data[hisPresIndex]) / data[hisRhoIndex];

	  gradK = sph_spiky_gradient(maxDist, R_ij);

	  gradP.x += coeff*gradK.x;
	  gradP.y += coeff*gradK.y;
	  gradP.z += coeff*gradK.z;

	  //Laplacien de la vitesse
	  coeff = 0;
	  if(data[hisRhoIndex])
	    coeff = particleMass * sph_viscosity_laplacian(maxDist, R_ij) / data[hisRhoIndex];

	  laplV.x += coeff*(data[myVelIndex] - data[hisVelIndex]);
	  laplV.y += coeff*(data[myVelIndex+1] - data[hisVelIndex+1]);
	  laplV.z += coeff*(data[myVelIndex+2] - data[hisVelIndex+2]);
	}
    }

  //calcul de l'accélération et de la vitesse
  // The gravity
  acc.x = acc.y = 0;
  if(data[myPosIndex+2] > 0.1)
    {
      acc.z = -9.8;
    }
  else
    {
      acc.z = -9.8;
      data[myVelIndex] = data[myVelIndex+1] = data[myVelIndex+2] = 0;
    }

  // The influences
  if(data[myRhoIndex])
    {
      acc.x += (coeff_mu*laplV.x - gradP.x)/data[myRhoIndex];
      acc.y += (coeff_mu*laplV.y - gradP.y)/data[myRhoIndex];
      acc.z += (coeff_mu*laplV.z - gradP.z)/data[myRhoIndex];
    }

  ++ cstep;

  //La vitesse
  float3 v0 = (float3)(data[myVelIndex], data[myVelIndex+1], data[myVelIndex+2]); //Save v_0

  data[myVelIndex] += acc.x*timestep;
  data[myVelIndex+1] += acc.y*timestep;
  data[myVelIndex+2] += acc.z*timestep;

  //La translation
  // x = 1/2*a*t^2 + v_0*t + x_0
  // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
  float time = timestep * cstep;
  float time_p = time - timestep;
  data[myPosIndex] += 0.5*acc.x*(time*time - time_p*time_p) + v0.x*timestep;
  data[myPosIndex+1] += 0.5*acc.y*(time*time - time_p*time_p) + v0.y*timestep;
  data[myPosIndex+2] += 0.5*acc.z*(time*time - time_p*time_p) + v0.z*timestep;
}
