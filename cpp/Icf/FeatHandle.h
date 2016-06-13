#pragma once
#include "BoxFeature.h"

namespace vca
{
  namespace icf
  {
    struct FeatHandle
    {
      FeatHandle(){};
      FeatHandle( BoxFeature::BoxType featType_, int featIdx_):featType(featType_),featIdx(featIdx_){}

      friend VCA_API istream& operator>>( istream& is, FeatHandle& tar );

      friend VCA_API ostream& operator<<( ostream& os, const FeatHandle& src );


      BoxFeature::BoxType featType;
      int featIdx;

    };
  }
}

