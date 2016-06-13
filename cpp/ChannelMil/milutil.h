
#pragma once
#include <valarray>
#include <iostream>
#include <opencv2/core/core.hpp>
#include "features/Icf/BoxSum.h"
#include "tracking/TrackingLogger.h"
#include "tracking/trackingMacros.h"
#define  sign(s)	((s > 0 ) ? 1 : ((s<0) ? -1 : 0))

typedef vca::icf::BoxFeature basisFtr;

//////////////////////////////////////////////////////////////////////////////////////////////////////


inline float
sigmoid(float x)
{
  return 1.0f / (1.0f + std::exp(-x));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


template<class T> void
resizeVec(std::vector<std::vector<T> > &v, int sz1, int sz2, T val = 0)
{
  v.resize(sz1);
  for (int k = 0; k < sz1; k++)
    v[k].resize(sz2, val);
};




