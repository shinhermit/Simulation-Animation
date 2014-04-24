
/// R_ij must be of size 3
float sph_poly6(float maxDist, float R_ij[])
{
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];
    float sqrMaxDist = maxDist*maxDist;

    if(sqrDist <= sqrMaxDist)
    {
        return pow(sqrMaxDist - sqrDist, 3) / (3.1415 * pow(maxDist, 8));
    }

    else
        return 0;
}

/// R_ij must be of size 3
/// res must be of size 3
void sph_poly6_gradient(float maxDist, float R_ij[], float res[])
{
    float coef;
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];
    float sqrMaxDist = maxDist*maxDist;

    res[0] = res[1] = res[2] = 0;

    if(sqrDist <= sqrMaxDist)
    {
        coef = -24*pow(sqrMaxDist - sqrDist, 2) / (3.1415 * pow(maxDist, 8));

        res[0] = coef*R_ij[0];
        res[1] = coef*R_ij[1];
        res[2] = coef*R_ij[2];
    }
}

/// R_ij must be of size 3
float sph_poly6_laplacian(float maxDist, float R_ij[])
{
    float sqrDist = R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2];
    float sqrMaxDist = maxDist*maxDist;

    if(sqrDist <= sqrMaxDist)
    {
        return -48*(sqrMaxDist - sqrDist)*(sqrMaxDist - 3*sqrDist) / (3.1415*pow(maxDist, 8));
    }

    else
        return 0;
}

/// R_ij must be of size 3
void sph_spiky_gradient(float maxDist, float R_ij[], float res[])
{
    float coef;
    float dist = sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);
    float q = dist/maxDist;

    res[0] = res[1] = res[2] = 0;

    if(dist <= maxDist)
    {
        coef = -30*pow(1-q, 2) / (3.1415*q*pow(maxDist, 4));


        res[0] = coef*R_ij[0];
        res[1] = coef*R_ij[1];
        res[2] = coef*R_ij[2];
    }
}

/// R_ij must be of size 3
float sph_viscosity_laplacian(float maxDist, float R_ij[])
{
    float dist = sqrt(R_ij[0]*R_ij[0] + R_ij[1]*R_ij[1] + R_ij[2]*R_ij[2]);
    float q = dist/maxDist;

    if(dist <= maxDist)
    {
        return 40*(1 - q) / (3.1415*pow(maxDist, 4));
    }

    else
        return 0;
}
