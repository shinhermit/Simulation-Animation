#ifndef GPU_SPHKERNELS
#define GPU_SPHKERNELS

float sph_poly6(float maxDist, float R_ij[]);
void sph_poly6_gradient(float maxDist, float R_ij[], float res[]);
float sph_poly6_laplacian(float maxDist, float R_ij[]);

void sph_spiky_gradient(float maxDist, float R_ij[], float res[]);

float sph_viscosity_laplacian(float maxDist, float R_ij[]);

#endif
