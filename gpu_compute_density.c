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

__kernel void compute_density(__global float * data, unsigned int nbItems,
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
	  R_ij.x = data[myPosIndex] - data[hisPosIndex];
	  R_ij.y = data[myPosIndex+1] - data[hisPosIndex+1];
	  R_ij.z = data[myPosIndex+2] - data[hisPosIndex+2];

	  density += particleMass * sph_poly6(maxDist, R_ij);
	}
    }

  data[myRhoIndex] = density;
  data[myPresIndex] = coeff_k*(density - refDensity);
}
