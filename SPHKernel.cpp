#include "SPHKernel.h"

SPHKernel::SPHKernel(const float & maxDist)
    :_maxDist(std::abs(maxDist)),
      _sqrMaxDist(_maxDist*_maxDist)
{}

SPHKernel::~SPHKernel()
{}

void SPHKernel::setMaxInfluenceDistance(const float & maxDist) throw(std::invalid_argument)
{
    if(maxDist < 0)
        throw std::invalid_argument("SPHKernel::setRadius: radius must be positive");

    _maxDist = maxDist;
    _sqrMaxDist = _maxDist*_maxDist;
}

float SPHKernel::getMaxInfluenceDistance()const
{
    return _maxDist;
}
