float sph_poly6(float maxDist, float3 R_ij)
{
    float sqrDist = R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z;
    float sqrMaxDist = maxDist*maxDist;

    if(sqrDist <= sqrMaxDist && maxDist != 0.)
    {
        return pown(sqrMaxDist - sqrDist, 3) / (3.1415 * pown(maxDist, 8));
    }

    else
        return 0.;
}

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

void compute_density(__global __read_only float * input, __global __write_only float * output, unsigned int nbItems,
		     float particleMass, float maxDist, float coeff_k, float refDensity)
{

  /***** Identify worker *****/
  unsigned int myId = get_global_id(0);

  /***** Do the work *****/
  float density = 0.;
  float3 R_ij;

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
      hisPosIndex = i*8;
      hisVelIndex = hisPosIndex + 3;
      hisRhoIndex = hisVelIndex + 3;
      hisPresIndex = hisRhoIndex + 1;

      if(i != myId)
        {
	  R_ij.x = input[myPosIndex] - input[hisPosIndex];
	  R_ij.y = input[myPosIndex+1] - input[hisPosIndex+1];
	  R_ij.z = input[myPosIndex+2] - input[hisPosIndex+2];

	  density += particleMass * sph_poly6(maxDist, R_ij);
	}
    }

  input[myRhoIndex] = density;
  input[myPresIndex] = coeff_k*(density - refDensity);
}

void compute_translation(__global __read_only float * input, __global __write_only float * output, unsigned int cstep, float timestep,
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

	  R_ij.x = input[myPosIndex] - input[hisPosIndex];
	  R_ij.y = input[myPosIndex+1] - input[hisPosIndex+1];
	  R_ij.z = input[myPosIndex+2] - input[hisPosIndex+2];

	  // Gradient de la pression
	  coeff = 0;
	  if(input[hisRhoIndex])
	    coeff = 0.5 * particleMass * (input[myPresIndex] + input[hisPresIndex]) / input[hisRhoIndex];

	  gradK = sph_spiky_gradient(maxDist, R_ij);

	  gradP.x += coeff*gradK.x;
	  gradP.y += coeff*gradK.y;
	  gradP.z += coeff*gradK.z;

	  //Laplacien de la vitesse
	  coeff = 0;
	  if(input[hisRhoIndex])
	    coeff = particleMass * sph_viscosity_laplacian(maxDist, R_ij) / input[hisRhoIndex];

	  laplV.x += coeff*(input[myVelIndex] - input[hisVelIndex]);
	  laplV.y += coeff*(input[myVelIndex+1] - input[hisVelIndex+1]);
	  laplV.z += coeff*(input[myVelIndex+2] - input[hisVelIndex+2]);
	}
    }

  //calcul de l'accélération et de la vitesse
  // The gravity
  acc.x = acc.y = 0;
  if(input[myPosIndex+2] > 0.1)
    {
      acc.z = -9.8;
    }
  else
    {
      acc.z = -9.8;
      input[myVelIndex] = input[myVelIndex+1] = input[myVelIndex+2] = 0;
    }

  // The influences
  if(input[myRhoIndex])
    {
      acc.x += (coeff_mu*laplV.x - gradP.x)/input[myRhoIndex];
      acc.y += (coeff_mu*laplV.y - gradP.y)/input[myRhoIndex];
      acc.z += (coeff_mu*laplV.z - gradP.z)/input[myRhoIndex];
    }

  ++ cstep;

  //La vitesse
  float3 v0 = (float3)(input[myVelIndex], input[myVelIndex+1], input[myVelIndex+2]); //Save v_0

  input[myVelIndex] += acc.x*timestep;
  input[myVelIndex+1] += acc.y*timestep;
  input[myVelIndex+2] += acc.z*timestep;

  //La translation
  // x = 1/2*a*t^2 + v_0*t + x_0
  // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
  float time = timestep * cstep;
  float time_p = time - timestep;
  input[myPosIndex] += 0.5*acc.x*(time*time - time_p*time_p) + v0.x*timestep;
  input[myPosIndex+1] += 0.5*acc.y*(time*time - time_p*time_p) + v0.y*timestep;
  input[myPosIndex+2] += 0.5*acc.z*(time*time - time_p*time_p) + v0.z*timestep;
}

void debug_fill_pos(__global __write_only float * output)
{
  unsigned int myId = get_global_id(0);

  unsigned int myPosIndex = myId*8;
  unsigned int myVelIndex = myPosIndex + 3;
  unsigned int myRhoIndex = myVelIndex + 3;
  unsigned int myPresIndex = myRhoIndex + 1;

  /* output[myPosIndex] += myId; */
  /* output[myPosIndex+1] += myId; */
  /* output[myPosIndex+2] += myId; */

  output[myPosIndex] += 3;
  output[myPosIndex+1] += 3;
  output[myPosIndex+2] += 3;

}

__kernel void gpu_step(__global __read_only float * input , __global __write_only float * output,  unsigned int nbItems, unsigned int cstep, float timestep,
				  float particleMass, float maxDist, float coeff_k, float coeff_mu, float refDensity)
{
  /* compute_density(input, output, nbItems, particleMass, maxDist, coeff_k, refDensity); */
  /* barrier(CLK_GLOBAL_MEM_FENCE); */
  /* compute_translation(input, output, cstep, timestep, nbItems, particleMass, maxDist, coeff_mu); */

  unsigned int myId = get_global_id(0);

  unsigned int myPosIndex = myId*8;
  unsigned int myVelIndex = myPosIndex + 3;
  unsigned int myRhoIndex = myVelIndex + 3;
  unsigned int myPresIndex = myRhoIndex + 1;

  /* output[myPosIndex] += myId; */
  /* output[myPosIndex+1] += myId; */
  /* output[myPosIndex+2] += myId; */

  output[myPosIndex] = 3;
  //output[myPosIndex+1] += 3;
  //output[myPosIndex+2] += 3;
  //debug_fill_pos(output);
}
