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
__kernel void compute_density(__global __read_only float * input, __global __write_only float * output, unsigned int nbItems,
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



__kernel void compute_translation(__global __read_only float* input, __global __write_only float * output, 
				  unsigned int nbItems, unsigned int cstep, float timestep, float particleMass,
				  float maxDist, float coeff_k, float coeff_mu, float refDensity) {
  unsigned int myId = get_global_id(0);
  
  float3 interactionForce, Rij, Vij, acc;
  float scalarForce, scalarPosition;
  float Qij;
  unsigned int i, otherIndex, myIndex;
  myIndex = myId * 8;
  
  interactionForce.x = 0;
  interactionForce.y = 0;
  interactionForce.z = 0;
  
  //in this loop, we compute the interaction force with all the others particles */
  for(i=0; i<nbItems; ++i) {
    //computing specific values
    //==========================
    otherIndex = i * 8;
    //Rij = Ri - Rj
    Rij.x = input[myIndex] - input[otherIndex];
    Rij.y = input[myIndex + 1] - input[otherIndex + 1];
    Rij.z = input[myIndex + 2] - input[otherIndex + 2];
    //Qij = sqrt(||Rij||) / h
    Qij =  sqrt(Rij.x*Rij.x + Rij.y*Rij.y + Rij.z*Rij.z) / maxDist;
    //Vij = Vi - Vj
    Vij.x = input[myIndex + 3] - input[otherIndex + 3];
    Vij.y = input[myIndex + 4] - input[otherIndex + 4];
    Vij.z = input[myIndex + 5] - input[otherIndex + 5];
    
    if(i!=myId) {
      //Computing interaction force
      //===================================
      
      /* First, we're going to compute the scalar that will be
       *  applicated to the interaction force*/
      scalarForce = (particleMass * (1-Qij)) / (M_PI_F * pown(maxDist, 4) * input[otherIndex+6]);
      scalarPosition = 15 * coeff_k * (input[myIndex+6] + input[otherIndex+6] - 2*refDensity);
      scalarPosition *= (1 - Qij) / Qij;
      
      /* Now we can compute the force for each axes */
      interactionForce.x += scalarForce * ( scalarPosition * Rij.x - 40 * coeff_mu * Vij.x);
      interactionForce.y += scalarForce * ( scalarPosition * Rij.y - 40 * coeff_mu * Vij.y);
      interactionForce.z += scalarForce * ( scalarPosition * Rij.z - 40 * coeff_mu * Vij.z);
    }
  }
  
  //Acceleration and speed computing
  //===================================
  
  //acceleration = (1/density)*interactionForces + gravity
  acc.x = (1/input[myIndex+6]) * interactionForce.x;
  acc.y = (1/input[myIndex+6]) * interactionForce.y;
  acc.z = (1/input[myIndex+6]) * interactionForce.z + 9.81;
  
  //Speed indice start at 3
  output[myIndex+3] = input[myIndex+3] + acc.x;
  output[myIndex+4] = input[myIndex+4] + acc.y;
  output[myIndex+5] = input[myIndex+5] + acc.z;
  
  
  //Computing Position
  //========================================
  ++cstep;
  
  float time = timestep * cstep;
  float previousTime = time - timestep;
  
  //Position = 1/2*a*(tEnd - tStart) + previousSpee*timeStep
  output[myIndex]= input[myIndex] + 0.5*acc.x*
  (time*time - previousTime * previousTime) + input[myIndex+3] * timestep;
  output[myIndex+1]= input[myIndex+1] + 0.5*acc.y*
  (time*time - previousTime * previousTime) + input[myIndex+4] * timestep;
  output[myIndex+2]= input[myIndex+2] + 0.5*acc.z*
  (time*time - previousTime * previousTime) + input[myIndex+5] * timestep;
				  }
				  