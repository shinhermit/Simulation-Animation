__kernel void maxList(__global __read_write float *kinematics,
		      float timestep)
{
  unsigned int index = get_global_id(0);
  unsigned int p = index*6;
  unsigned int v = p+3;
  float step = *timestep;
  float vel_p;

  const float G = 9.8;

  vel_p = kinematics[v+2];
  kinematics[v+2] -= G * step;

  kinematics[p] += kinematics[v] * step;
  kinematics[p+1] += kinematics[v+1] * step;
  kinematics[p+2] += (vel_p + kinematics[v+2])/2 * step;
}
