float sph_poly6(float h, float3 R_ij)
{
    float r2 = R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z;
    float r = sqrt(r2);
    float h2 = h*h;

    if(r <= h)
    {
        return 315.*pown(h2 - r2, 3) / (64. * M_PI_F * pown(h, 9));
    }

    else
        return 0.;
}

float3 sph_spiky(float h, float3 R_ij)
{
  float3 res;
  float coef;
    float r = sqrt(R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z);

    res.x = res.y = res.z = 0.;

    if(r <= h)
    {
        coef = 45.*pown(h-r, 3) / (r*M_PI_F*pown(h, 6));


        res.x = coef*R_ij.x;
        res.y = coef*R_ij.y;
        res.z = coef*R_ij.z;
    }

    return res;
}

float sph_visco(float h, float3 R_ij)
{
    float r = sqrt(R_ij.x*R_ij.x + R_ij.y*R_ij.y + R_ij.z*R_ij.z);

    if(r <= h)
    {
        return 45.*(h - r) / (M_PI_F*pown(h, 6));
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

  output[myRhoIndex] = density;
  output[myPresIndex] = coeff_k*(density - refDensity);
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

	  gradK = sph_spiky(maxDist, R_ij);

	  gradP.x += coeff*gradK.x;
	  gradP.y += coeff*gradK.y;
	  gradP.z += coeff*gradK.z;

	  //Laplacien de la vitesse
	  coeff = 0;
	  if(input[hisRhoIndex])
	    coeff = particleMass * sph_visco(maxDist, R_ij) / input[hisRhoIndex];

	  laplV.x += coeff*(input[myVelIndex] - input[hisVelIndex]);
	  laplV.y += coeff*(input[myVelIndex+1] - input[hisVelIndex+1]);
	  laplV.z += coeff*(input[myVelIndex+2] - input[hisVelIndex+2]);
	}
    }

  //calcul de l'accélération et de la vitesse
  // The gravity
  float3 v0;
  acc.x = acc.y = 0;
  if(input[myPosIndex+2] > 0.1)
    {
      acc.z = -9.8;
      v0 = (float3)(input[myVelIndex], input[myVelIndex+1], input[myVelIndex+2]);
    }
  else
    {
      acc.z = 0;
      v0 = (float3)(0,0,0);
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
  output[myVelIndex] = v0.x + acc.x*timestep;
  output[myVelIndex+1] = v0.y + acc.y*timestep;
  output[myVelIndex+2] = v0.z + acc.z*timestep;

  //La translation
  // x = 1/2*a*t^2 + v_0*t + x_0
  // Dx = x2-x1 = 1/2*a*(t2^2 - t1^2) + v_0*(t2 - t1)
  float time = timestep * cstep;
  float time_p = time - timestep;
  output[myPosIndex] = input[myPosIndex] + 0.5*acc.x*(time*time - time_p*time_p) + v0.x*timestep;
  output[myPosIndex+1] = input[myPosIndex+1] + 0.5*acc.y*(time*time - time_p*time_p) + v0.y*timestep;
  output[myPosIndex+2] = input[myPosIndex+2] + 0.5*acc.z*(time*time - time_p*time_p) + v0.z*timestep;
}

void debug_fill_pos(__global __write_only float * vector)
{
  unsigned int myId = get_global_id(0);

  unsigned int myPosIndex = myId*8;
  unsigned int myVelIndex = myPosIndex + 3;
  unsigned int myRhoIndex = myVelIndex + 3;
  unsigned int myPresIndex = myRhoIndex + 1;

  vector[myPosIndex] = 30;
  vector[myPosIndex+1] = 30;
  vector[myPosIndex+2] = 30;
}

__kernel void gpu_step(__global __read_only float * input , __global __write_only float * output,  unsigned int nbItems, unsigned int cstep, float timestep,
				  float particleMass, float maxDist, float coeff_k, float coeff_mu, float refDensity)
{
  compute_density(input, output, nbItems, particleMass, maxDist, coeff_k, refDensity);
  barrier(CLK_GLOBAL_MEM_FENCE);
  compute_translation(input, output, cstep, timestep, nbItems, particleMass, maxDist, coeff_mu);

  /* debug_fill_pos(output); */
}
