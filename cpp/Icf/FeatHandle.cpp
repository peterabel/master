#include "FeatHandle.h"
#include "featureTypeStringConverter.h"

namespace vca
{
  namespace icf
  {
    istream& operator>>(istream& is, FeatHandle& tar)
    {
      string boxTypeString;
      is >> boxTypeString  >> tar.featIdx; 
      tar.featType = FeatureTypeStringConverter::string2BoxType( boxTypeString );
      return is;
    }

    ostream& operator<<(ostream& os, const FeatHandle& src)
    {
      os << FeatureTypeStringConverter::boxType2string( src.featType) << " " << src.featIdx;
      return os;
    }

  }
}